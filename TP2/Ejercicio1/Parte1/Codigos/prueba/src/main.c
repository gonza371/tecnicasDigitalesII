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

// --- Campos de Bits ---
#define RCC_AFIOEN (1 << 0)
#define RCC_IOPAEN (1 << 2)
#define RCC_IOPCEN (1 << 4)
#define GPIOC13    (1UL << 13)
#define GPIOA7     (1UL << 7)

// --- Registros del SysTick ---
#define SysTick_BASE 0xE000E010
#define SysTick_CTRL *(volatile uint32_t *)(SysTick_BASE + 0x00)
#define SysTick_LOAD *(volatile uint32_t *)(SysTick_BASE + 0x04)
#define SysTick_VAL  *(volatile uint32_t *)(SysTick_BASE + 0x08)

#define SysTick_CTRL_ENABLE    (1 << 0)
#define SysTick_CTRL_TICKINT   (1 << 1)
#define SysTick_CTRL_CLKSOURCE (1 << 2)
#define SysTick_CTRL_COUNTFLAG (1 << 16)

volatile uint32_t tick;

// --- Rutinas de Servicio de Interrupción (ISR) ---

// ISR del SysTick
void SysTick_Handler(void)
{
    tick++;
}

// ISR del Botón en PA1 (EXTI Línea 1)
void EXTI1_IRQHandler(void)
{
    // Verificar si la interrupción provino de la línea EXTI1
    if (EXTI_PR & (1 << 1))
    {
        // Limpiar el flag escribiendo un 1 en el Pending Register
        EXTI_PR |= (1 << 1);
        
        // Alternar el estado del LED en PA7
        GPIOA_ODR ^= GPIOA7; 
    }
}

// --- Funciones de Tiempo ---

void systick_init_ms(void)
{
    tick = 0;
    SysTick_CTRL &= ~SysTick_CTRL_CLKSOURCE; // HCLK/8
    SysTick_LOAD = 999; 
    SysTick_VAL = 0; 
    SysTick_CTRL |= SysTick_CTRL_TICKINT | SysTick_CTRL_ENABLE; 
}

void delay_ms_bloqueante(uint32_t ms)
{
    uint32_t tiempo_inicio = tick + ms;
    while (tick != tiempo_inicio);
}

// Retardo bloqueante por bucle, como en la versión original del TP1
void delay_bucle(volatile uint32_t iteraciones)
{
    while (iteraciones > 0)
    {
        iteraciones--;
    }
}

// --- Función Principal ---

void main(void)
{
    // ---------------------------------------------------------
    // ETAPA 1: RCC (Relojes)
    // ---------------------------------------------------------
    RCC_APB2ENR |= RCC_AFIOEN | RCC_IOPAEN | RCC_IOPCEN;

    // ---------------------------------------------------------
    // ETAPA 2: GPIO (Pines físicos)
    // ---------------------------------------------------------
    // PC13 como salida push-pull (LED del TP1)
    GPIOC_CRH &= 0xFF0FFFFF;
    GPIOC_CRH |= 0x00200000;
    
    // PA7 como salida push-pull y PA1 como entrada pull-up/down
    GPIOA_CRL &= 0x0FFFFF0F; 
    GPIOA_CRL |= 0x20000080;
    GPIOA_ODR |= (1 << 1);

    // ---------------------------------------------------------
    // ETAPA 3: AFIO (Multiplexor de funciones alternativas)
    // ---------------------------------------------------------
    AFIO_EXTICR1 &= ~(0x000000F0); 

    // ---------------------------------------------------------
    // ETAPA 4: EXTI (Controlador de interrupciones externas)
    // ---------------------------------------------------------
    EXTI_IMR  |= (1 << 1);
    EXTI_FTSR |= (1 << 1);

    // ---------------------------------------------------------
    // ETAPA 5: NVIC (Controlador anidado de interrupciones)
    // ---------------------------------------------------------
    NVIC_ISER0 |= (1 << 7); 

    // Inicializar la base de tiempo con SysTick
    systick_init_ms();

    // ---------------------------------------------------------
    // BUCLE PRINCIPAL (Programa base)
    // ---------------------------------------------------------
    while (1)
    {
        GPIOC_ODR &= ~GPIOC13;
        delay_ms_bloqueante(500); 
        GPIOC_ODR |= GPIOC13;
        delay_ms_bloqueante(500); 
    }
}