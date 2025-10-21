#include "limits.h"

#include "bsp_gpio.h"
#include "bsp_pins.h"
#include "stm32f446xx.h"

#define DEBOUNCE_TICKS 5 // ≈5 ms at 1 kHz; bump to 8–10 if needed

typedef struct {
    uint8_t cnt;
    uint8_t stable; // 0/1 debounced state
    uint8_t last_sample; // 0/1 previous raw sample
} Deb;

static Deb s_min_db[3]; // one MIN switch per axis

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
    if (d->cnt >= DEBOUNCE_TICKS) {
        d->stable = sample;
        d->cnt = 0;
    }
    return d->stable;
}

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
            // seed debouncer with current raw level
            s_min_db[i].cnt = 0;
            s_min_db[i].last_sample = read_active(&LIM_MIN[i]) ? 1 : 0;
            s_min_db[i].stable = s_min_db[i].last_sample;
        }
    }
}

void limits_poll_tick(void) {
    for (int i = 0; i < 3; ++i) {
        if (!LIM_MIN[i].port)
            continue;
        uint8_t raw = read_active(&LIM_MIN[i]) ? 1 : 0;
        deb_tick(&s_min_db[i], raw);
    }
}

bool limits_min_pressed(axis_t a) {
    return s_min_db[(int)a].stable != 0;
}

bool limits_block_neg(axis_t a) {
    // policy: block negative motion when MIN is debounced-pressed
    return limits_min_pressed(a);
}
