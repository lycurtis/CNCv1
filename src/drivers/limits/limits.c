#include "limits.h"

#include "bsp_gpio.h"
#include "bsp_pins.h"
#include "stm32f446xx.h"

typedef struct {
    GPIO_TypeDef* port;
    uint8_t pin;
    uint8_t active_low; // 1 = pressed when pin reads 0
} LimHw;

static const LimHw LIM_MIN[3] = {
        {X_MIN_PORT, X_MIN_PIN, X_MIN_ACTIVE_LOW},
        {Y_MIN_PORT, Y_MIN_PIN, Y_MIN_ACTIVE_LOW},
        {Z_MIN_PORT, Z_MIN_PIN, Z_MIN_ACTIVE_LOW},
};

static inline bool read_active(const LimHw* h) {
    const bool hi = ((h->port->IDR >> h->pin) & 1U) != 0;
    return h->active_low ? !hi : hi;
}

void limits_init_min(void) {
    // enable clocks and set input+PU for each configured MIN pin
    for (int i = 0; i < 3; ++i) {
        if (LIM_MIN[i].port) {
            bsp_gpio_en(LIM_MIN[i].port);
            bsp_gpio_in_pu(LIM_MIN[i].port, LIM_MIN[i].pin);
        }
    }
}

bool limits_min_pressed(axis_t a) {
    return read_active(&LIM_MIN[(int)a]);
}

bool limits_block_neg(axis_t a) {
    // Minimal policy (step 1): block negative motion whenever MIN is pressed
    return limits_min_pressed(a);
}