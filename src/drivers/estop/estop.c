#include "estop.h"

#include "bsp_gpio.h"
#include "bsp_pins.h"
#include "stm32f446xx.h"

#define ESTOP_DEBOUNCE_TICKS 5 // ≈5 ms

typedef struct {
    uint8_t cnt;
    uint8_t stable;
    uint8_t last_sample;
} Deb;

static Deb s_estop_db;
static volatile uint8_t s_latched = 0;

static inline uint8_t deb_tick(Deb* d, uint8_t sample) {
    if (sample == d->stable) {
        d->cnt = 0;
        d->last_sample = sample;
        return d->stable;
    }
    if (sample != d->last_sample) {
        d->last_sample = sample;
        d->cnt = 1;
    } else if (d->cnt < 255) {
        d->cnt++;
    }
    if (d->cnt >= ESTOP_DEBOUNCE_TICKS) {
        d->stable = sample;
        d->cnt = 0;
    }
    return d->stable;
}

static inline bool read_active(GPIO_TypeDef* port, uint8_t pin, bool active_high) {
    const bool hi = ((port->IDR >> pin) & 1UL) != 0;
    return active_high ? hi : !hi;
}

void estop_init(void) {
    bsp_gpio_en(ESTOP_PORT);
    bsp_gpio_in_pd(ESTOP_PORT, ESTOP_PIN);
    s_latched = 0;

    uint8_t raw = read_active(ESTOP_PORT, ESTOP_PIN, ESTOP_ACTIVE_HIGH != 0) ? 1 : 0;
    s_estop_db.cnt = 0;
    s_estop_db.last_sample = raw;
    s_estop_db.stable = raw;
}

void estop_poll_tick(void) {
    uint8_t raw = read_active(ESTOP_PORT, ESTOP_PIN, ESTOP_ACTIVE_HIGH != 0) ? 1 : 0;
    uint8_t deb = deb_tick(&s_estop_db, raw);
    if (deb)
        s_latched = 1;
}

bool estop_latched(void) {
    return s_latched != 0;
}

void estop_clear(void) {
    s_latched = 0;
}