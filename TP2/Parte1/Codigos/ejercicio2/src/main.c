#include <stdint.h>

// --- Direcciones Base ---
#define RCC_BASE   0x40021000
#define GPIOC_BASE 0x40011000
#define GPIOA_BASE 0x40010800
#define AFIO_BASE  0x40010000
#define EXTI_BASE  0x40010400
#define NVIC_BASE  0xE000E100

// --- Registros de Periféricos ---
#define RCC_APB2ENR  *(volatile uint32_t *)(RCC_BASE + 0x18)
#define GPIOC_CRH    *(volatile uint32_t *)(GPIOC_BASE + 0x04)
#define GPIOA_CRL    *(volatile uint32_t *)(GPIOA_BASE + 0x00)
#define GPIOC_ODR    *(volatile uint32_t *)(GPIOC_BASE + 0x0C)
#define GPIOA_ODR    *(volatile uint32_t *)(GPIOA_BASE + 0x0C)

#define AFIO_EXTICR1 *(volatile uint32_t *)(AFIO_BASE + 0x08)
#define EXTI_IMR     *(volatile uint32_t *)(EXTI_BASE + 0x00)
#define EXTI_FTSR    *(volatile uint32_t *)(EXTI_BASE + 0x0C)
#define EXTI_PR      *(volatile uint32_t *)(EXTI_BASE + 0x14)
#define NVIC_ISER0   *(volatile uint32_t *)(NVIC_BASE + 0x00)

// --- Registros del ADC1 ---
#define ADC1_BASE  0x40012400
#define ADC1_SR    *(volatile uint32_t *)(ADC1_BASE + 0x00)
#define ADC1_CR2   *(volatile uint32_t *)(ADC1_BASE + 0x08)
#define ADC1_SMPR1 *(volatile uint32_t *)(ADC1_BASE + 0x0C)
#define ADC1_SMPR2 *(volatile uint32_t *)(ADC1_BASE + 0x10)
#define ADC1_SQR3  *(volatile uint32_t *)(ADC1_BASE + 0x34)
#define ADC1_DR    *(volatile uint32_t *)(ADC1_BASE + 0x4C)

// --- Campos de Bits ---
#define RCC_AFIOEN (1 << 0)
#define RCC_IOPAEN (1 << 2)
#define RCC_IOPCEN (1 << 4)
#define RCC_ADC1EN (1 << 9)

#define GPIOC13    (1UL << 13)

// <-- CAMBIO DE PINES: La barra se movió para usar PA3 a PA6 -->
#define GPIOA3     (1UL << 3)  // Nuevo LED 1
#define GPIOA4     (1UL << 4)  // Nuevo LED 2
#define GPIOA5     (1UL << 5)  // Nuevo LED 3
#define GPIOA6     (1UL << 6)  // Nuevo LED 4

#define ADC_CR2_ADON    (1 << 0)
#define ADC_CR2_CAL     (1 << 2)  // Bit de calibración
#define ADC_CR2_EXTSEL  (7 << 17) // Selección de trigger (111 = SWSTART)
#define ADC_CR2_EXTTRIG (1 << 20) // Habilitar trigger externo
#define ADC_CR2_SWSTART (1 << 22)
#define ADC_CR2_TSVREFE (1 << 23)
#define ADC_SR_EOC      (1 << 1)

// Variable de estado global para conmutar canales
volatile uint8_t modo_adc = 0; // 0: Temperatura (CH16), 1: Potenciómetro (CH1)

// --- ISR del Botón (Pin PA2) ---
void EXTI2_IRQHandler(void)
{
    if (EXTI_PR & (1 << 2))
    {
        // <-- CORRECCIÓN: Asignación directa con '=' para no borrar otras banderas -->
        EXTI_PR = (1 << 2); 
        modo_adc = !modo_adc; 
    }
}

void delay_bucle(volatile uint32_t iteraciones) {
    while (iteraciones > 0) iteraciones--;
}

void main(void)
{
    // 1. Clocks
    RCC_APB2ENR |= RCC_AFIOEN | RCC_IOPAEN | RCC_IOPCEN | RCC_ADC1EN;

    // 2. Configurar GPIO
    GPIOC_CRH &= 0xFF0FFFFF;
    GPIOC_CRH |= 0x00200000;
    
    // <-- CORRECCIÓN: Máscaras de GPIOA_CRL para la nueva barra (PA3 a PA6) -->
    // PA7 -> Sin tocar (F)
    // PA6, PA5, PA4, PA3 -> Salidas push-pull (2)
    // PA2 -> Entrada Pull-up (8)
    // PA1 -> Entrada Analógica (0)
    // PA0 -> Sin tocar (F)
    GPIOA_CRL &= 0xF000000F; 
    GPIOA_CRL |= 0x02222800; 
    GPIOA_ODR |= (1 << 2);   

    // 3. AFIO y EXTI
    AFIO_EXTICR1 &= ~(0x00000F00); 
    EXTI_IMR  |= (1 << 2);
    EXTI_FTSR |= (1 << 2); 
    NVIC_ISER0 |= (1 << 8); 

    // 4. Inicializar ADC1
    ADC1_CR2 |= ADC_CR2_ADON;    // Despierta el ADC
    delay_bucle(5000);           // Tiempo de estabilización (Tstab)
    ADC1_CR2 |= ADC_CR2_ADON;    // Enciende definitivamente el conversor
    
    // <-- CORRECCIÓN: Calibración obligatoria del hardware ADC -->
    ADC1_CR2 |= ADC_CR2_CAL;
    while(ADC1_CR2 & ADC_CR2_CAL); // Espera a que el hardware termine de calibrarse

    ADC1_CR2 |= ADC_CR2_TSVREFE; // Energiza el sensor de temperatura interno
    
    // <-- CORRECCIÓN: Configurar el multiplexor para disparar el ADC por software -->
    ADC1_CR2 |= ADC_CR2_EXTSEL | ADC_CR2_EXTTRIG;

    ADC1_SMPR1 |= (0x7 << 18); 
    ADC1_SMPR2 |= (0x7 << 3);  

    uint16_t valor_adc = 0;

    while (1)
    {
        GPIOC_ODR ^= GPIOC13;

        if (modo_adc == 0) {
            ADC1_SQR3 = 16; 
        } else {
            ADC1_SQR3 = 1;  
        }

        // Iniciar conversión
        ADC1_CR2 |= ADC_CR2_SWSTART;

        // Esperar EOC (End Of Conversion)
        while ((ADC1_SR & ADC_SR_EOC) == 0);

        valor_adc = ADC1_DR;

        // Apagamos los 4 pines de la barra
        GPIOA_ODR &= ~(GPIOA6 | GPIOA5 | GPIOA4 | GPIOA3);

        // Encendido progresivo acumulativo
        if (valor_adc > 819)  GPIOA_ODR |= GPIOA3; // Primer nivel
        if (valor_adc > 1638) GPIOA_ODR |= GPIOA4; // Segundo nivel
        if (valor_adc > 2457) GPIOA_ODR |= GPIOA5; // Tercer nivel
        if (valor_adc > 3276) GPIOA_ODR |= GPIOA6; // Cuarto nivel (Máximo)

        delay_bucle(30000); 
    }
}