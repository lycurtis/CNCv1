#include "stepgen_pwm_tim3.h"

// // X Axis
// X_STEP_PORT GPIOA
// X_STEP_PIN 6U // PA6 (TIM3_CH1) AF2
// X_STEP_AF_VAL 2UL // AF2 -> TIM3_CH1
// X_DIR_PORT GPIOB
// X_DIR_PIN 0U // PB0
// X_EN_PORT GPIOB
// X_EN_PIN 1U // PB1 (note: LOW = enable)

void stepgen_init(){
    // Clock enable
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

    // Timer 3 Enable
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

    // Configure gpio
    gpio_af_pp_hs(X_STEP_PORT, X_STEP_PIN, X_STEP_AF_VAL);
    gpio_out_pp_hs(X_DIR_PORT, X_DIR_PIN);
    gpio_out_pp_hs(X_EN_PORT, X_EN_PIN);

    // Default state for safety (TMC2209 Disabled: Enable_Pin = HIGH)
    X_EN_PORT->BSRR |= (1UL << X_EN_PIN); // Set PB1 to HIGH

    // TIM3 Configuration (Timer base: 1 MHz tick, 1 kHz period, 50% duty cycle )
    TIM3->CR1 = 0; // clear bits
    TIM3->PSC = TIM_PSC_1MHz; // 1 Mhz timer clock
    TIM3->ARR = 1000 - 1; // period => 1 kHz PWM (With a 1 MHz tick, ARR = 999 → period = 1000 ticks → 1 kHz.)
    TIM3->CCR1 = (TIM3->ARR + 1) / 2; // 50% duty 
    // CCR1 = 500 makes duty ≈ 50%. In PWM mode 1, the channel is HIGH while CNT < CCR1 and LOW afterwards (polarity permitting)
    
    // Channel config (PWM mode + preload)
    TIM3->CCMR1 &= ~(TIM_CCMR1_CC1S | TIM_CCMR1_OC1M); //CC1S=00 selects output mode (not input capture).
    TIM3->CCMR1 |= (6UL << TIM_CCMR1_OC1M_Pos); // OC1M=110 selects PWM mode 1
    TIM3->CCMR1 |= TIM_CCMR1_OC1PE; // OC1PE enables a shadow register for CCR1 so updates latch on update events (no mid-cycle glitches)
    TIM3->CCER  &= ~TIM_CCER_CC1P; // active-high polarity
    TIM3->CCER  |=  TIM_CCER_CC1E; // enable CH1 output (turns on the channel output driver)

    TIM3->CR1   |=  TIM_CR1_ARPE; // TIM3_ARR register is buffered (ARPE=1 gives ARR a shadow register too (same reason as CCR1))

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

void stepgen_start(void){
    TIM3->CR1 |= TIM_CR1_CEN; // Counter enabled
}

void stepgen_stop(void){
    TIM3->CR1 &= ~TIM_CR1_CEN; // Counter disabled
}

void stepgen_set_hz(uint32_t hz){
    // Guard and compute ARR from 1 MHz base: ARR = 1e6/hz - 1
    if(hz == 0){
        stepgen_stop();
        return;
    }
    uint32_t arr = (1000000UL / hz);
    if(arr == 0 ){
        arr = 1; // clamp
    }
    arr -= 1UL;

    TIM3->ARR = (uint16_t)arr;
    TIM3->CCR1 = (uint16_t)((arr + 1UL) >> 1); // 50% duty
    TIM3->EGR = TIM_EGR_UG; // latch safety at next update

}

void stepgen_dir(bool fwd){
    if(fwd){
        X_DIR_PORT->BSRR = (1UL << X_DIR_PIN);
    }
    else{
        X_DIR_PORT->BSRR = (1UL << (X_DIR_PIN+16)); // reset = LOW
    }
}

void stepgen_enable(bool enable_low_active){
    /* TMC2209 EN pin is active LOW. (pass TRUE to enable) */
    if(enable_low_active){
        X_EN_PORT->BSRR = (1UL << (X_EN_PIN + 16)); // LOW ==> Enable
    }
    else{
        X_EN_PORT->BSRR = (1UL << X_EN_PIN); // HIGH ==> Disable
    }
}