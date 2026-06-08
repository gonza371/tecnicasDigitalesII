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

// <-- CORRECCIÓN: Registros robustos de 32 bits para prioridades -->
#define NVIC_IPR1    *(volatile uint32_t *)(NVIC_BASE + 0x304) // Controla IRQ 4 a 7
#define NVIC_IPR2    *(volatile uint32_t *)(NVIC_BASE + 0x308) // Controla IRQ 8 a 11
#define SCB_SHPR3    *(volatile uint32_t *)(0xE000ED20)        // Controla prioridad del SysTick

// --- Campos de Bits ---
#define RCC_AFIOEN (1 << 0)
#define RCC_IOPAEN (1 << 2)
#define RCC_IOPCEN (1 << 4)
#define GPIOC13    (1UL << 13)
#define GPIOA6     (1UL << 6) // LED Botón Baja Prioridad
#define GPIOA5     (1UL << 5) // LED Botón Alta Prioridad
#define GPIOA4     (1UL << 4) // LED Normalidad en PA4

// --- Registros del SysTick ---
#define SysTick_BASE 0xE000E010
#define SysTick_CTRL *(volatile uint32_t *)(SysTick_BASE + 0x00)
#define SysTick_LOAD *(volatile uint32_t *)(SysTick_BASE + 0x04)
#define SysTick_VAL  *(volatile uint32_t *)(SysTick_BASE + 0x08)

#define SysTick_CTRL_ENABLE    (1 << 0)
#define SysTick_CTRL_TICKINT   (1 << 1)
#define SysTick_CTRL_CLKSOURCE (1 << 2)

volatile uint32_t tick;

// --- Funciones de Tiempo ---

void systick_init_ms(void)
{
    tick = 0;
    SysTick_CTRL &= ~SysTick_CTRL_CLKSOURCE; 
    SysTick_LOAD = 999; 
    SysTick_VAL = 0; 
    SysTick_CTRL |= SysTick_CTRL_TICKINT | SysTick_CTRL_ENABLE; 
}

void delay_ms_bloqueante(uint32_t ms)
{
    uint32_t tiempo_inicio = tick;
    while ((tick - tiempo_inicio) < ms); 
}

// --- Rutinas de Servicio de Interrupción (ISR) ---

void SysTick_Handler(void)
{
    tick++;
}

// Botón en PA1 (EXTI1) - PRIORIDAD BAJA -> Titila PA6 5 veces
void EXTI1_IRQHandler(void)
{
    if (EXTI_PR & (1 << 1))
    {
        EXTI_PR = (1 << 1); 
        
        for (int i = 0; i < 10; i++)
        {
            GPIOA_ODR ^= GPIOA6; 
            delay_ms_bloqueante(200);
        }
        
        GPIOA_ODR &= ~GPIOA6; 
    }
}

// Botón en PA2 (EXTI2) - PRIORIDAD ALTA -> Titila PA5 5 veces
void EXTI2_IRQHandler(void)
{
    if (EXTI_PR & (1 << 2))
    {
        EXTI_PR = (1 << 2); 
        
        for (int i = 0; i < 10; i++)
        {
            GPIOA_ODR ^= GPIOA5; 
            delay_ms_bloqueante(200);
        }
        
        GPIOA_ODR &= ~GPIOA5;
    }
}

// --- Función Principal ---

void main(void)
{
    // 1. RCC (Relojes)
    RCC_APB2ENR |= RCC_AFIOEN | RCC_IOPAEN | RCC_IOPCEN;

    // 2. GPIO
    // PC13 (LED placa) como salida
    GPIOC_CRH &= 0xFF0FFFFF;
    GPIOC_CRH |= 0x00200000;
    
    // Configurar PA6, PA5, PA4 (salidas) y PA2, PA1 (entradas)
    GPIOA_CRL &= 0xF000F00F; 
    GPIOA_CRL |= 0x02220880; 
    
    // Activamos resistencias Pull-up internas en PA1 y PA2
    GPIOA_ODR |= (1 << 1) | (1 << 2);

    // 3. AFIO (Multiplexor)
    AFIO_EXTICR1 &= ~(0x00000FF0); 

    // 4. EXTI
    EXTI_IMR  |= (1 << 1) | (1 << 2);
    EXTI_FTSR |= (1 << 1) | (1 << 2);

    // <-- CORRECCIÓN: 5. NVIC PRIORIDADES (32 bits reales) -->
    
    // a) Garantizar que SysTick tenga la prioridad máxima (0x00)
    SCB_SHPR3 &= ~(0xFF << 24);

    // b) Limpiar las prioridades previas de EXTI1 (IRQ 7) y EXTI2 (IRQ 8)
    NVIC_IPR1 &= ~(0xFF << 24); // Limpia bits 24 a 31
    NVIC_IPR2 &= ~(0xFF << 0);  // Limpia bits 0 a 7

    // c) Asignar prioridades (recordando que el número menor es la prioridad más alta)
    // Usamos los bits superiores de cada byte porque STM32 lee los 4 bits más significativos
    NVIC_IPR2 |= (0x40 << 0);  // PA2 (EXTI2) - ALTA Prioridad (0x40)
    NVIC_IPR1 |= (0x80 << 24); // PA1 (EXTI1) - BAJA Prioridad (0x80)

    // Jerarquía resultante: SysTick (0x00) > PA2 (0x40) > PA1 (0x80)

    // 6. NVIC ENABLE
    NVIC_ISER0 |= (1 << 7) | (1 << 8); 

    systick_init_ms();

    while (1)
    {
        // --- Tarea de Normalidad (Background Task) ---
        GPIOA_ODR ^= GPIOA4; 
        GPIOC_ODR ^= GPIOC13;
        delay_ms_bloqueante(500); 
    }
}