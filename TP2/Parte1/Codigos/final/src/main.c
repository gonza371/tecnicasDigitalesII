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
#define NVIC_IPR     ((volatile uint8_t *)(NVIC_BASE + 0x400))

// --- Campos de Bits ---
#define RCC_AFIOEN (1 << 0)
#define RCC_IOPAEN (1 << 2)
#define RCC_IOPCEN (1 << 4)
#define GPIOC13    (1UL << 13)
#define GPIOA7     (1UL << 7) // LED Normalidad
#define GPIOA6     (1UL << 6) // LED Botón Baja Prioridad
#define GPIOA5     (1UL << 5) // LED Botón Alta Prioridad

// --- Registros del SysTick ---
#define SysTick_BASE 0xE000E010
#define SysTick_CTRL *(volatile uint32_t *)(SysTick_BASE + 0x00)
#define SysTick_LOAD *(volatile uint32_t *)(SysTick_BASE + 0x04)
#define SysTick_VAL  *(volatile uint32_t *)(SysTick_BASE + 0x08)

#define SysTick_CTRL_ENABLE    (1 << 0)
#define SysTick_CTRL_TICKINT   (1 << 1)
#define SysTick_CTRL_CLKSOURCE (1 << 2)

volatile uint32_t tick;

// --- Funciones de Tiempo (Declaradas antes para poder usarlas en las ISR) ---

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
    uint32_t tiempo_inicio = tick + ms;
    // Mientras esperamos, el SysTick interrumpe periódicamente este bucle
    while (tick != tiempo_inicio);
}

// --- Rutinas de Servicio de Interrupción (ISR) ---

void SysTick_Handler(void)
{
    // SysTick tiene prioridad 0x00 (la máxima). Siempre actualizará el tiempo.
    tick++;
}

// Botón en PA1 (EXTI1) - PRIORIDAD BAJA -> Titila PA6 5 veces
void EXTI1_IRQHandler(void)
{
    if (EXTI_PR & (1 << 1))
    {
        // Limpiamos el flag primero para que futuras pulsaciones se registren correctamente
        EXTI_PR |= (1 << 1); 
        
        // Encendemos y apagamos 5 veces (10 cambios de estado en total)
        for (int i = 0; i < 10; i++)
        {
            GPIOA_ODR ^= GPIOA6; 
            delay_ms_bloqueante(200); // Demora visible de 200ms
        }
        
        // Nos aseguramos de que el LED quede apagado al salir
        GPIOA_ODR &= ~GPIOA6; 
    }
}

// Botón en PA2 (EXTI2) - PRIORIDAD ALTA -> Titila PA5 5 veces
void EXTI2_IRQHandler(void)
{
    if (EXTI_PR & (1 << 2))
    {
        // Limpiamos el flag
        EXTI_PR |= (1 << 2); 
        
        // Encendemos y apagamos 5 veces
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
    
    // Configurar PA7, PA6 y PA5 como SALIDAS push-pull (2)
    // Configurar PA2 y PA1 como ENTRADAS pull-up/down (8)
    GPIOA_CRL &= 0x000FF00F; 
    GPIOA_CRL |= 0x22200880; 
    
    // Activamos resistencias Pull-up internas en PA1 y PA2
    GPIOA_ODR |= (1 << 1) | (1 << 2);

    // 3. AFIO (Multiplexor)
    AFIO_EXTICR1 &= ~(0x00000FF0); 

    // 4. EXTI
    EXTI_IMR  |= (1 << 1) | (1 << 2);
    EXTI_FTSR |= (1 << 1) | (1 << 2);

    // 5. NVIC PRIORIDADES (Ajustadas para permitir que SysTick no se congele)
    NVIC_IPR[7] = 0x20; // Prioridad BAJA para Botón PA1 (IRQ 7)
    NVIC_IPR[8] = 0x10; // Prioridad ALTA para Botón PA2 (IRQ 8)
    // Nota: SysTick mantiene su prioridad por defecto que es 0x00 (Máxima)

    // 6. NVIC ENABLE
    NVIC_ISER0 |= (1 << 7) | (1 << 8); 

    systick_init_ms();

    while (1)
    {
        // --- Tarea de Normalidad (Background Task) ---
        GPIOA_ODR ^= GPIOA7; 
        GPIOC_ODR ^= GPIOC13;
        delay_ms_bloqueante(500); 
    }
}