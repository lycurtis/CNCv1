#include "bsp_usart2_debug.h"

#include "bsp_gpio.h"
#include "bsp_pins.h"
#include "stm32f4xx.h"

// PA 2 TX
// PA 3 RX

void dbg_uart_init(uint32_t pclk1_hz, uint32_t baud) {
    // Clocks
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    // MODER AF7
    // GPIOA->MODER |= GPIO_MODER_MODE2_1; // PA2
    // GPIOA->MODER |= GPIO_MODER_MODE3_1; // PA3
    DBG_TX_PORT->MODER &= ~(3UL << (DBG_TX_PIN * 2));
    DBG_TX_PORT->MODER |= (2UL << (DBG_TX_PIN * 2)); // AF MODE
    DBG_RX_PORT->MODER &= ~(3UL << (DBG_RX_PIN * 2));
    DBG_RX_PORT->MODER |= (2UL << (DBG_RX_PIN * 2));

    // AF7 on both pins
    // if (DBG_TX_PIN < 8){
    //     DBG_TX_PORT->AFR[0] |=  (7UL << (DBG_TX_PIN*4));
    // }
    // else{
    //     DBG_TX_PORT->AFR[1] |=  (7UL << ((DBG_TX_PIN-8)*4));
    // }

    // if(DBG_RX_PIN < 8){
    //     DBG_RX_PORT->AFR[0] |=  (7UL << (DBG_RX_PIN*4));
    // }
    // else{
    //     DBG_RX_PORT->AFR[1] |=  (7UL << ((DBG_RX_PIN-8)*4));
    // }
    DBG_TX_PORT->AFR[0] |= (7UL << (DBG_TX_PIN * 4));
    DBG_RX_PORT->AFR[0] |= (7UL << (DBG_RX_PIN * 4));

    // Configuration: 8N1 (8 data bits, no parity bit, and 1 stop bit), oversampling by 16
    USART2->CR1 |= USART_CR1_TE | USART_CR1_RE; // Transmit and Receive enable

    // BRR = pclk/baud (oversampling 16) rounded
    uint32_t brr = (pclk1_hz + (baud / 2u)) / baud;

    USART2->BRR = brr;

    USART2->CR1 |= USART_CR1_UE; // enable
}

void dbg_putc(char c) {
    while (!(USART2->SR & USART_SR_TXE)) {
    } // Wait until transmit buffer empty
    USART2->DR = (c & 0xFF);
}

void dbg_write(const char* s) {
    while (*s) {
        dbg_putc(*s++);
    }
}

int dbg_getc_nonblock(void) {
    if (USART2->SR & USART_SR_RXNE) {
        return (int)(USART2->DR & 0xFF);
    }
    return -1;
}
