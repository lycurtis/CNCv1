#pragma once

#include <stdint.h>
#include "stm32f446xx.h"

/**
 * Enable AHB1 clock for a given GPIO port.
 * Safe to call multiple times.
 */
void bsp_gpio_en(GPIO_TypeDef *port);

/**
 * Configure pin as Push-Pull, High-Speed, No-Pull (Output).
 */
void bsp_gpio_out_pp_hs(GPIO_TypeDef* port, uint32_t pin);

/**
 * Configure pin as Alternate Function, Push-Pull, High-Speed, No-Pull,
 * and set AF value (0..15).
 */
void bsp_gpio_af_pp_hs(GPIO_TypeDef* port, uint32_t pin, uint8_t af_val);
