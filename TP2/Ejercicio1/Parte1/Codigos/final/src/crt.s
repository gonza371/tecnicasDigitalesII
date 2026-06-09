.syntax unified 
.cpu cortex-m3
.thumb
.extern _esstack

.section .text.manejadores // definimos la seccion text.manejadores 

// función básica para manejar los punteros 
.thumb_func 
.weak Default_Handler
Default_Handler:
    b .             /* Bucle infinito */

// definir los vectores ejemplo
.weak NMI_Handler
.thumb_set NMI_Handler, Default_Handler
.weak SysTick_Handler
.thumb_set SysTick_Handler, Default_Handler
.weak HardFault_Handler
.thumb_set HardFault_Handler, Default_Handler
.weak WWDG_IRQHandler
.thumb_set WWDG_IRQHandler, Default_Handler
.weak PVD_IRQHandler
.thumb_set PVD_IRQHandler, Default_Handler
.weak TAMPER_IRQHandler
.thumb_set TAMPER_IRQHandler, Default_Handler
.weak RTC_IRQHandler
.thumb_set RTC_IRQHandler, Default_Handler
.weak FLASH_IRQHandler
.thumb_set FLASH_IRQHandler, Default_Handler
.weak RCC_IRQHandler
.thumb_set RCC_IRQHandler, Default_Handler
.weak EXTI0_IRQHandler
.thumb_set EXTI0_IRQHandler, Default_Handler
.weak EXTI1_IRQHandler
.thumb_set EXTI1_IRQHandler, Default_Handler
.weak EXTI2_IRQHandler
.thumb_set EXTI2_IRQHandler, Default_Handler
.weak EXTI3_IRQHandler
.thumb_set EXTI3_IRQHandler, Default_Handler
.weak EXTI4_IRQHandler
.thumb_set EXTI4_IRQHandler, Default_Handler

// vectores 
.section .isr_vector,"a",%progbits
.word _esstack
.word _reset
.word NMI_Handler         /* 2:  NMI (Non-Maskable Interrupt) */
.word HardFault_Handler   /* 3:  HardFault (falla grave) */
.word 0                   /* 4:  MemManage (falla de memoria) */
.word 0                   /* 5:  BusFault (falla de bus) */
.word 0                   /* 6:  UsageFault (falla de uso) */
.word 0                   /* 7:  Reservado */
.word 0                   /* 8:  Reservado */
.word 0                   /* 9:  Reservado */
.word 0                   /* 10: Reservado */
.word 0                   /* 11: SVCall (Llamada al Supervisor) */
.word 0                   /* 12: Debug Monitor */
.word 0                   /* 13: Reservado */
.word 0                   /* 14: PendSV (Interrupción pendiente del sistema) */
.word SysTick_Handler     /* 15: El handler del SysTick */
.word WWDG_IRQHandler
.word PVD_IRQHandler
.word TAMPER_IRQHandler
.word RTC_IRQHandler
.word FLASH_IRQHandler
.word RCC_IRQHandler
.word EXTI0_IRQHandler
.word EXTI1_IRQHandler
.word EXTI2_IRQHandler
.word EXTI3_IRQHandler
.word EXTI4_IRQHandler

// declaramos la función _reset dentro de la seccion text.reset
.section .text.reset
.thumb_func
.global _reset
_reset:
    bl main
    b .
    
