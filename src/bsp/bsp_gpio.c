#include "bsp_gpio.h"

void bsp_gpio_en(GPIO_TypeDef* port) {
    if (port == GPIOA) {
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    } else if (port == GPIOB) {
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    } else if (port == GPIOC) {
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
    } else if (port == GPIOD) {
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
    } else if (port == GPIOE) {
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
    } else if (port == GPIOF) {
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;
    } else if (port == GPIOG) {
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;
    } else if (port == GPIOH) {
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOHEN;
    }
}

void bsp_gpio_in_pu(GPIO_TypeDef* port, uint8_t pin) { // input pull up
    // Enable PU: MODER=00 (input), PUPDR=01 (pull-up)
    uint32_t pos = (uint32_t)pin * 2U;
    port->MODER &= ~(3UL << pos);
    port->PUPDR &= ~(3UL << pos);
    port->PUPDR |= (1UL << pos);
}

void bsp_gpio_in_pd(GPIO_TypeDef* port, uint8_t pin) { // input pull down
    // Enable PU: MODER=00 (input), PUPDR=10 (pull-down)
    uint32_t pos = (uint32_t)pin * 2UL;
    port->MODER &= ~(3UL << pos);
    port->PUPDR &= ~(3UL << pos);
    port->PUPDR |= (2UL << pos);
}

void bsp_gpio_out_pp_hs(GPIO_TypeDef* port, uint32_t pin) {
    /*
    Recall each pin for MODER takes 2 bits. Therefore if we want pin 6 to output mode
    MODER6 will occupy bits 12,13. Hence the pin*2. pin=6 ==> 6*2= 12. Little endianess
    */
    port->MODER &= ~(3UL << (pin * 2)); // clear bits
    port->MODER |= (1UL << (pin * 2)); // mode: output mode (01)
    port->OTYPER &= ~(1UL << pin); // type: output push pull (00 reset state)
    port->OSPEEDR |= (3UL << (pin * 2)); // speed: High speed (11)
    port->PUPDR &= ~(3UL << (pin * 2)); // resistor: no pull (00 reset state)
}

void bsp_gpio_af_pp_hs(GPIO_TypeDef* port, uint32_t pin, uint8_t af_val) {
    port->MODER &= ~(3UL << (pin * 2)); // clear bits
    port->MODER |= (2UL << (pin * 2)); // mode: alternate function mode (10)

    /*
    Configure Altnerate function selection (Use Table to make selection)
    pins 0-7 ==> AFRL(ow) = AFR[0]
    pins 8-15 ==> AFRH(igh) = AFR[1]
    */
    volatile uint32_t* afr = (pin < 8) ? &port->AFR[0] : &port->AFR[1];
    /* because each pin for AFR takes 4 bits. Hence the pin*4*/
    uint32_t sh = (pin % 8) * 4; // %8 needed to offset for pins > 7
    *afr &= ~(0xFUL << sh); // clear bits
    *afr |= ((uint32_t)af_val << sh); // map pin according to selection
    port->OTYPER &= ~(1UL << pin); // type: output push pull (00 reset state)
    port->OSPEEDR |= (3UL << (pin * 2)); // speed: High speed (11)
    port->PUPDR &= ~(3UL << (pin * 2)); // resistor: no pull (00 reset state)
}