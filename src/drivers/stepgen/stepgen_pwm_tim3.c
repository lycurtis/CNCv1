#include "stepgen_pwm_tim3.h"

#include <stddef.h>

#include "bsp_gpio.h"
#include "bsp_pins.h"
#include "limits.h"

/*
On STM32F446 with SYSCLK=180MHz, APB1 prescaler = 4
==> APB1 bus clock = 180/4 = 45MHz
Timers on STM32 have special rules:
-Case 1: If APB prescaler = 1 --> timer clock = APB clock
-Case 2: If APB precaaler > 1 --> timer clock = 2*APB clock
==> TIM3clk=2*APB1=90MHz.
Now the counter clock frequency (CK_CNT) = fck_psc/(PSC[15:0]+1)
we want CK_CNT = 1MHz = 90MHz(PSC + 1)
Therefore PSC = (90MHz/1MHz) - 1 = 89  TIM_PSC_1MHz (90UL - 1UL)
*/
#define TIM_PSC_1MHz (90UL - 1UL)

static volatile uint32_t steps_remaining[3] = {0, 0, 0};
static volatile uint8_t moving_mask = 0; // bit0=X, bit1=Y, bit2=Z
static volatile uint8_t dir_is_fwd[3] = {1, 1, 1};

typedef struct {
    // STEP (AF = TIM3 CHn)
    GPIO_TypeDef* step_port;
    uint8_t step_pin;
    uint8_t step_af;
    uint8_t ch;

    // DIR / EN (EN is active-LOW on TMC2209)
    GPIO_TypeDef* dir_port;
    uint8_t dir_pin;

    GPIO_TypeDef* en_port;
    uint8_t en_pin;
} AxisHw;

static const AxisHw AXIS_HW[] = {
        // X-Axis
        {X_STEP_PORT, X_STEP_PIN, X_STEP_AF_VAL, 1, X_DIR_PORT, X_DIR_PIN, X_EN_PORT, X_EN_PIN},

        // Y-Axis
        {Y_STEP_PORT, Y_STEP_PIN, Y_STEP_AF_VAL, 2, Y_DIR_PORT, Y_DIR_PIN, Y_EN_PORT, Y_EN_PIN},

        // Z-Axis
        {Z_STEP_PORT, Z_STEP_PIN, Z_STEP_AF_VAL, 3, Z_DIR_PORT, Z_DIR_PIN, Z_EN_PORT, Z_EN_PIN},
};

static inline const AxisHw* ainfo(axis_t a) {
    return &AXIS_HW[(int)a];
}

// Channel enable/disable
static inline void ch_enable(uint8_t ch, bool on) {
    uint32_t m = (ch == 1) ? TIM_CCER_CC1E : (ch == 2) ? TIM_CCER_CC2E : TIM_CCER_CC3E;
    if (on) {
        TIM3->CCER |= m;
    } else {
        TIM3->CCER &= ~m;
    }
}

/* Map CH -> CCR pointer (array index 1..3 valid) */
static volatile uint32_t* const CCRn[4] = {NULL, &TIM3->CCR1, &TIM3->CCR2, &TIM3->CCR3};

static void init_axis_gpio_and_channel(axis_t a) {
    const AxisHw* h = ainfo(a);

    // GPIO clocks + pinmux
    bsp_gpio_en(h->step_port);
    bsp_gpio_en(h->dir_port);
    bsp_gpio_en(h->en_port);

    bsp_gpio_af_pp_hs(h->step_port, h->step_pin, h->step_af); // STEP (AF -> TIM3 CHn)
    bsp_gpio_out_pp_hs(h->dir_port, h->dir_pin); // DIR
    bsp_gpio_out_pp_hs(h->en_port, h->en_pin); // EN

    // Default state for safety (TMC2209 Disabled: Enable_Pin = HIGH)
    h->en_port->BSRR = (1UL << h->en_pin);

    /* Channel config: PWM mode 1 + preload, active high */
    if (h->ch == 1) {
        TIM3->CCMR1 &= ~(TIM_CCMR1_CC1S | TIM_CCMR1_OC1M);
        TIM3->CCMR1 |= (6UL << TIM_CCMR1_OC1M_Pos) | TIM_CCMR1_OC1PE;
        TIM3->CCER &= ~TIM_CCER_CC1P;
    } else if (h->ch == 2) {
        TIM3->CCMR1 &= ~(TIM_CCMR1_CC2S | TIM_CCMR1_OC2M);
        TIM3->CCMR1 |= (6UL << TIM_CCMR1_OC2M_Pos) | TIM_CCMR1_OC2PE;
        TIM3->CCER &= ~TIM_CCER_CC2P;
    } else { /* ch == 3 */
        TIM3->CCMR2 &= ~(TIM_CCMR2_CC3S | TIM_CCMR2_OC3M);
        TIM3->CCMR2 |= (6UL << TIM_CCMR2_OC3M_Pos) | TIM_CCMR2_OC3PE;
        TIM3->CCER &= ~TIM_CCER_CC3P;
    }

    ch_enable(h->ch, true);
}

static inline uint8_t axis_bit(axis_t a) {
    return (uint8_t)(1UL << (int)a);
}

static inline bool any_moving(void) {
    return moving_mask != 0;
}

/*------------ Public API ---------------*/

void stepgen_init_all(void) {
    // Timer 3 Clock Enable
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

    // Timer 3 DMA/interrupt enable register (Enable update interrupt, NVIC)
    TIM3->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM3_IRQn);

    /**
     * Timer 3 Base Configuration
     * 1 MHz tick, 1kHz default period, ARPE on
     */
    TIM3->CR1 = 0;
    TIM3->PSC = TIM_PSC_1MHz; // 1 MHz timer clock
    TIM3->ARR = 1000 - 1; // period => 1 kHz PWM (With a 1 MHz tick, ARR = 999 → period = 1000 ticks
                          // → 1 kHz.)
    TIM3->CR1 |= TIM_CR1_ARPE; // Don’t immediately change the timer period when ARR is updated
                               // instead wait until the next update event

    // Init all axes (pins + per-channel PWM config)
    init_axis_gpio_and_channel(AXIS_X);
    init_axis_gpio_and_channel(AXIS_Y);
    init_axis_gpio_and_channel(AXIS_Z);

    /* 50% duty on all channels initially */
    uint16_t ccr = (uint16_t)((TIM3->ARR + 1U) >> 1);
    TIM3->CCR1 = ccr;
    TIM3->CCR2 = ccr;
    TIM3->CCR3 = ccr;

    // Update generation
    TIM3->EGR = TIM_EGR_UG; // Update Generation: load ARR/CCR1, reset CNT
    /*
    Reinitialize the counter and generates an update of the registers. Note that the prescaler
    counter is cleared too (anyway the prescaler ratio is not affected). The counter is cleared
    if the center-aligned mode is selected or if DIR=0 (upcounting), else it takes the
    auto-reload value (TIMx_ARR) if DIR=1 (downcounting).

    Forces an update event: copies ARR/CCR1 from their preload buffers into the active registers
    and resets the counter to 0. Without this, your preloads wouldn’t take effect until the first
    natural rollover
    */
}

void stepgen_start_all(void) {
    TIM3->CR1 |= TIM_CR1_CEN; // Counter enabled
}

void stepgen_stop_all(void) {
    TIM3->CR1 &= ~TIM_CR1_CEN; // Counter disabled
}

void stepgen_enable(axis_t a, bool enable_low_active) {
    const AxisHw* h = ainfo(a);

    /* TMC2209 EN pin is active LOW. (pass TRUE to enable) */
    if (enable_low_active) {
        h->en_port->BSRR = (1UL << (h->en_pin + 16)); // LOW => enable
    } else {
        h->en_port->BSRR = (1UL << h->en_pin); // HIGH ==> Disable
    }
}

void stepgen_dir(axis_t a, bool fwd) {
    const AxisHw* h = ainfo(a);
    dir_is_fwd[(int)a] = fwd;

    if (fwd) {
        h->dir_port->BSRR = (1UL << h->dir_pin);
    } else {
        h->dir_port->BSRR = (1UL << (h->dir_pin + 16)); // reset = LOW
    }
}

/**
 * Shared frequency (TIM3): set ARR from 1 MHz base, and 50% duty for selected axis
   ARR is shared -> all axes run at the same Hz
 */
void stepgen_set_hz(axis_t a, uint32_t hz) {
    // Guard and compute ARR from 1 MHz base: ARR = 1e6/hz - 1
    if (hz == 0UL) {
        stepgen_stop_all();
        return;
    }
    uint32_t arr = (1000000UL / hz); // 1 MHz base
    if (arr == 0UL) {
        arr = 1UL; // clamp
    }
    arr -= 1UL;

    TIM3->ARR = (uint16_t)arr;

    // Selected axis keeps 50% duty
    const AxisHw* h = ainfo(a);
    *CCRn[h->ch] = (uint16_t)((arr + 1U) >> 1);

    TIM3->EGR = TIM_EGR_UG; /* latch ARR/CCR */
}

bool stepgen_busy(axis_t a) {
    return (moving_mask & axis_bit(a)) != 0;
}

void stepgen_move_n(axis_t a, uint32_t steps, uint32_t hz) {
    if (steps == 0 || hz == 0) {
        return;
    }

    // guard starts when moving negative and MIN is pressed
    if (!dir_is_fwd[(int)a] && limits_block_neg(a)) {
        return; // ignore unsafe command
    }

    stepgen_set_hz(a, hz); // shared timer freq for now

    steps_remaining[(int)a] = steps;
    moving_mask |= axis_bit(a);

    ch_enable(ainfo(a)->ch, true);
    TIM3->CR1 |= TIM_CR1_CEN;
}

void TIM3_IRQHandler(void) {
    if (TIM3->SR & TIM_SR_UIF) {
        TIM3->SR &= ~TIM_SR_UIF; // clear update flag

        // Each update event = one PWM period = one step
        for (int i = 0; i < 3; ++i) {
            if (steps_remaining[i]) {

                // hard stop if moving toward MIN and MIN is asserted
                if (!dir_is_fwd[i] && limits_block_neg((axis_t)i)) {
                    uint8_t ch = AXIS_HW[i].ch;
                    ch_enable(ch, false);
                    moving_mask &= ~(1u << i);
                    steps_remaining[i] = 0;
                    continue; // skip decrement
                }

                if (--steps_remaining[i] == 0) {
                    // Finished this axis → disable its channel
                    uint8_t ch = AXIS_HW[i].ch;
                    ch_enable(ch, false);
                    moving_mask &= ~(1u << i);
                }
                // NOTE: in this simple version, we assume only ONE axis is moving at a time.
                // If multiple were armed, they'd all decrement together (shared frequency).
            }
        }

        // Stop timer if nothing left
        if (!any_moving()) {
            TIM3->CR1 &= ~TIM_CR1_CEN;
        }
    }
}