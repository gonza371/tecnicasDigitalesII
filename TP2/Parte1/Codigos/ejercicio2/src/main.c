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
#define GPIOA4     (1UL << 4)  // Nuevo LED 1
#define GPIOA5     (1UL << 5)  // Nuevo LED 2
#define GPIOA6     (1UL << 6)  // Nuevo LED 3
#define GPIOA7     (1UL << 7)  // Nuevo LED 4

#define ADC_CR2_ADON    (1 << 0)
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
        EXTI_PR |= (1 << 2); // Limpiar bandera pendiente
        modo_adc = !modo_adc; // Invertir el modo de medición
    }
}

void delay_bucle(volatile uint32_t iteraciones) {
    while (iteraciones > 0) iteraciones--;
}

void main(void)
{
    // 1. Clocks: GPIOA, GPIOC, AFIO y el periférico ADC1
    RCC_APB2ENR |= RCC_AFIOEN | RCC_IOPAEN | RCC_IOPCEN | RCC_ADC1EN;

    // 2. Configurar GPIO
    // PC13 como salida (LED de latido de la placa)
    GPIOC_CRH &= 0xFF0FFFFF;
    GPIOC_CRH |= 0x00200000;
    
    // Configuración milimétrica de GPIOA_CRL:
    // PA7, PA6, PA5, PA4 -> Salidas push-pull 2MHz (Valor: 2)
    // PA3 -> Sin tocar / por defecto (Valor: 0)
    // PA2 -> Entrada con Pull-up para el botón (Valor: 8)
    // PA1 -> Entrada analógica para el potenciómetro (Valor: 0)
    // PA0 -> Sin tocar / por defecto (Valor: 0)
    GPIOA_CRL &= 0x0000F00F; // Máscara para limpiar pines del 1, 2, y del 4 al 7
    GPIOA_CRL |= 0x22220800; // Escribe las configuraciones correspondientes
    GPIOA_ODR |= (1 << 2);   // Activa la resistencia Pull-up del botón en PA2

    // 3. Configurar AFIO y EXTI para el botón (PA2 -> EXTI2)
    AFIO_EXTICR1 &= ~(0x00000F00); 
    EXTI_IMR  |= (1 << 2);
    EXTI_FTSR |= (1 << 2); // Disparo por flanco de bajada (pulsión)
    NVIC_ISER0 |= (1 << 8); // Habilitar interrupción EXTI2 en el NVIC

    // 4. Inicializar ADC1
    ADC1_CR2 |= ADC_CR2_ADON;    // Despierta el ADC
    delay_bucle(5000);           // Tiempo de estabilización por manual (Tstab)
    ADC1_CR2 |= ADC_CR2_ADON;    // Enciende definitivamente el conversor
    ADC1_CR2 |= ADC_CR2_TSVREFE; // Energiza el sensor de temperatura interno
    
    // Configurar máximo tiempo de muestreo para garantizar lecturas estables
    ADC1_SMPR1 |= (0x7 << 18); // Canal 16 (Temperatura)
    ADC1_SMPR2 |= (0x7 << 3);  // Canal 1 (Potenciómetro)

    uint16_t valor_adc = 0;

    while (1)
    {
        // LED indicador de que el programa sigue vivo
        GPIOC_ODR ^= GPIOC13;

        // Selección de Secuencia de canal según el modo actual
        if (modo_adc == 0) {
            ADC1_SQR3 = 16; // Canal 16 interno
        } else {
            ADC1_SQR3 = 1;  // Canal 1 (Pin PA1)
        }

        // Iniciar la conversión por software
        ADC1_CR2 |= ADC_CR2_SWSTART;

        // Esperar el fin de conversión (End Of Conversion)
        while ((ADC1_SR & ADC_SR_EOC) == 0);

        // Almacenar el valor de 12 bits medido
        valor_adc = ADC1_DR;

        // --- Lógica del Indicador de Barra (4 LEDs) ---
        // Primero apagamos los 4 pines de la barra usando una máscara limpia
        GPIOA_ODR &= ~(GPIOA7 | GPIOA6 | GPIOA5 | GPIOA4);

        // Encendido progresivo acumulativo según los umbrales calculados
        if (valor_adc > 819)  GPIOA_ODR |= GPIOA4; // Primer nivel
        if (valor_adc > 1638) GPIOA_ODR |= GPIOA5; // Segundo nivel
        if (valor_adc > 2457) GPIOA_ODR |= GPIOA6; // Tercer nivel
        if (valor_adc > 3276) GPIOA_ODR |= GPIOA7; // Cuarto nivel (Máximo)

        delay_bucle(30000); // Pequeña ventana de tiempo antes de la próxima muestra
    }
}