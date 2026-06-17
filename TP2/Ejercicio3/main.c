#include "stm32f10x.h"

Variables globales volátiles para la interrupción del SysTick
volatile uint32_t msTicks = 0;       // Usado para la función Delay_ms bloqueante
volatile uint32_t globalTicks = 0;   // Usado para temporizar el LED sin bloquear el código

// Bandera para el estado del botón (0 = Sensor de temperatura interno, 1 = Potenciómetro externo)
volatile uint8_t flag_sensor_ext = 0; 

// =========================================================
// RUTINAS DE SERVICIO DE INTERRUPCIÓN (ISR)
// =========================================================

void SysTick_Handler(void) {
    if (msTicks > 0) {
        msTicks--;
    }
    globalTicks++; // Incremento continuo para tareas no bloqueantes
}

void EXTI0_IRQHandler(void) {
    // Si la interrupción fue disparada por la línea 0 (Botón en PB0)
    if(EXTI->PR & EXTI_PR_PR0) {
        flag_sensor_ext = !flag_sensor_ext; // Alternar fuente de ADC
        EXTI->PR = EXTI_PR_PR0;             // Limpiar flag de interrupción
    }
}

// =========================================================
// FUNCIONES DE CONFIGURACIÓN Y RETARDO
// =========================================================

void Delay_ms(uint32_t delay) {
    msTicks = delay;
    while(msTicks != 0); // Espera hasta que el SysTick decremente la variable a 0
}

void Init_SysTick(void) {
    // Configura el SysTick para generar una interrupción cada 1 ms
    SysTick_Config(SystemCoreClock / 1000);
}

void Init_GPIO(void) {
    // Habilitar relojes para AFIO, GPIOA, GPIOB y GPIOC
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN | 
                    RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN;

    // PC13 (LED parpadeante) - Output Push-Pull a 2MHz
    GPIOC->CRH &= ~(0xF << 20);
    GPIOC->CRH |= (0x2 << 20);

    // PA0 a PA4 (5 LEDs indicadores de ADC) - Output Push-Pull a 2MHz
    GPIOA->CRL &= ~(0xFFFFF);
    GPIOA->CRL |= 0x22222;

    // PA5 (Potenciómetro para ADC) - Entrada Analógica (Modo 0x0)
    GPIOA->CRL &= ~(0xF << 20);

    // PB0 (Botón) - Input Pull-up
    GPIOB->CRL &= ~(0xF << 0);
    GPIOB->CRL |= (0x8 << 0);
    GPIOB->ODR |= (1 << 0); // Resistencia Pull-up activada
}

void Init_EXTI(void) {
    // Mapear PB0 a la línea EXTI0 en los registros AFIO
    AFIO->EXTICR[0] &= ~(0xF << 0);
    AFIO->EXTICR[0] |= (0x1 << 0); // 0001: Port B pin 0

    EXTI->IMR |= EXTI_IMR_MR0;     // Desenmascarar línea 0
    EXTI->FTSR |= EXTI_FTSR_TR0;   // Configurar disparo por flanco de bajada

    // Habilitar la interrupción externa 0 en el NVIC
    NVIC_EnableIRQ(EXTI0_IRQn);
}

void Init_ADC(void) {
    // Habilitar reloj del periférico ADC1
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    
    // Encender el ADC
    ADC1->CR2 |= ADC_CR2_ADON;
    Delay_ms(1); // Tiempo de estabilización recomendado
    
    // Habilitar el sensor de temperatura interno
    ADC1->CR2 |= ADC_CR2_TSVREFE;
    
    // Rutina de Calibración
    ADC1->CR2 |= ADC_CR2_RSTCAL;
    while(ADC1->CR2 & ADC_CR2_RSTCAL);
    ADC1->CR2 |= ADC_CR2_CAL;
    while(ADC1->CR2 & ADC_CR2_CAL);
}

// =========================================================
// FUNCIONES DE CONTROL
// =========================================================

uint16_t Read_ADC(uint8_t channel) {
    // Configurar el canal deseado en el secuenciador principal
    ADC1->SQR3 = channel;
    
    // Iniciar conversión por software
    ADC1->CR2 |= ADC_CR2_SWSTART;
    
    // Esperar a que el flag EOC (End of Conversion) se levante (Polling)
    while(!(ADC1->SR & ADC_SR_EOC));
    
    // Leer y devolver el dato
    return ADC1->DR;
}

void Actualizar_LEDs_ADC(uint16_t adc_val) {
    // Apagar todos los 5 LEDs previamente encendidos en PA0-PA4
    GPIOA->BRR = 0x1F; 
    
    // Lógica de encendido escalonado (El ADC es de 12 bits: valores de 0 a 4095)
    // 4096 / 6 = aprox. pasos de 682
    if(adc_val > 682)  GPIOA->BSRR = GPIO_BSRR_BS0;
    if(adc_val > 1364) GPIOA->BSRR = GPIO_BSRR_BS1;
    if(adc_val > 2046) GPIOA->BSRR = GPIO_BSRR_BS2;
    if(adc_val > 2728) GPIOA->BSRR = GPIO_BSRR_BS3;
    if(adc_val > 3410) GPIOA->BSRR = GPIO_BSRR_BS4;
}

// =========================================================
// PROGRAMA PRINCIPAL
// =========================================================

int main(void) {
    // Inicializar el reloj principal (normalmente definido en system_stm32f10x.c)
    SystemInit();
    
    // Inicialización de Periféricos y hardware
    Init_SysTick();
    Init_GPIO();
    Init_EXTI();
    Init_ADC();

    uint32_t last_led_toggle = 0;
    uint16_t valor_adc = 0;

    while (1) {
        // Tarea 1: LED Parpadeante no bloqueante (Requisito TP1)
        // Se ejecuta cada 500 ms sin frenar el flujo del while(1)
        if (globalTicks - last_led_toggle >= 500) {
            last_led_toggle = globalTicks;
            GPIOC->ODR ^= GPIO_ODR_ODR13; // Alternar estado de PC13
        }

        // Tarea 2: Lectura del ADC dependiendo del flag accionado por el botón
        if (flag_sensor_ext) {
            valor_adc = Read_ADC(5);  // Canal 5 (PA5) - Potenciómetro externo
        } else {
            valor_adc = Read_ADC(16); // Canal 16 - Sensor de temperatura interno
        }

        // Tarea 3: Representación en la barra de 5 LEDs
        Actualizar_LEDs_ADC(valor_adc);
        
        // Retardo pequeño para evitar oscilaciones rápidas y permitir estabilización
        Delay_ms(50); 
    }
}