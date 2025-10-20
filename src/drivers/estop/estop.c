#include "estop.h"

#include "bsp_gpio.h"
#include "bsp_pins.h"
#include "stm32f446xx.h"

static volatile uint8_t s_latched = 0;

void estop_init(void) {
    bsp_gpio_en(ESTOP_PORT);
    bsp_gpio_in_pd(ESTOP_PORT, ESTOP_PIN);
    s_latched = 0;
}

static inline bool read_active(GPIO_TypeDef* port, uint8_t pin, bool active_high) {
    const bool hi = ((port->IDR >> pin) & 1UL) != 0;
    return active_high ? hi : !hi;
}

bool estop_pressed_raw(void) {
    return read_active(ESTOP_PORT, ESTOP_PIN, ESTOP_ACTIVE_HIGH != 0);
}

void estop_latch_if_pressed_fast(void) {
    if (estop_pressed_raw()) {
        s_latched = 1;
    }
}

bool estop_latched(void) {
    return s_latched != 0;
}

void estop_clear(void) {
    s_latched = 0;
}