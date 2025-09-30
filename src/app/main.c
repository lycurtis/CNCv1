#include "main.h"

int main(void){
    system_clock_init();

    // Enable GPIOA clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    // Set PA5 as general-purpose output
    GPIOA->MODER &= ~(3u << (5*2));
    GPIOA->MODER |=  (1u << (5*2));

    while(1){
        GPIOA->BSRR |= (1<<5);  // Set the pin PA5
        delay(5000000);
		GPIOA->BSRR |= ((1<<5) <<16);  // Reset pin PA5
        delay(5000000);
    }
}
