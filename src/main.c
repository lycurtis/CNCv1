#include "stm32f4xx.h"  // CMSIS device header

int main(void) {
    // Default HSI clock from system_stm32f4xx.c is fine to start.

    // Enable GPIOA clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    // Set PA5 as general-purpose output
    GPIOA->MODER &= ~(3u << (5*2));
    GPIOA->MODER |=  (1u << (5*2));

    for (;;) {
        GPIOA->ODR ^= (1u << 5);
        for (volatile uint32_t i = 0; i < 300000; ++i) __NOP();
    }
}
