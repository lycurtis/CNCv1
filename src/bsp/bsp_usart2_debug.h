#pragma once

#include <stdint.h>

void dbg_uart_init(uint32_t pclk1_hz, uint32_t baud);
void dbg_putc(char c);
void dbg_write(const char* s);
int dbg_getc_nonblock(void);