#include "stepgen_pwm_tim3.h"

// // X Axis
// X_STEP_PORT GPIOA
// X_STEP_PIN 6U // PA6 (TIM3_CH1) AF2
// X_STEP_AF_VAL 2UL // AF2 -> TIM3_CH1
// X_DIR_PORT GPIOB
// X_DIR_PIN 0U // PB0
// X_EN_PORT GPIOB
// X_EN_PIN 1U // PB1 (note: LOW = enable)

static inline void gpio_out_pp_hs(GPIO_TypeDef* port, uint32_t pin){
    /*
    Recall each pin for MODER takes 2 bits. Therefore if we want pin 6 to output mode
    MODER6 will occupy bits 12,13. Hence the pin*2. pin=6 ==> 6*2= 12. Little endianess 
    */ 
    port->MODER &= ~(3UL << (pin*2)); // clear bits
    port->MODER |= (1UL << (pin*2)); // mode: output mode (01)
    port->OTYPER &= ~(1UL << pin); // type: output push pull (00 reset state)
    port->OSPEEDR |= (3UL << (pin*2)); // speed: High speed (11)
    port->PUPDR &= ~(3UL << (pin*2)); // resistor: no pull (00 reset state)
}

static inline void gpio_af_pp_hs(GPIO_TypeDef* port, uint32_t pin, uint8_t af_val){
    port->MODER &= ~(3UL << (pin*2)); // clear bits
    port->MODER |= (2UL << (pin*2));// mode: alternate function mode (10)

    /* 
    Configure Altnerate function selection (Use Table to make selection)
    pins 0-7 ==> AFRL(ow) = AFR[0]
    pins 8-15 ==> AFRH(igh) = AFR[1]  
    */
    volatile uint32_t* afr = (pin < 8) ? &port->AFR[0] : &port->AFR[1];
    /* because each pin for AFR takes 4 bits. Hence the pin*4*/
    uint32_t sh = (pin%8)*4; // %8 needed to offset for pins > 7
    *afr &= ~(0xFUL << sh); // clear bits
    *afr |=  ((uint32_t)af_val << sh); // map pin according to selection
    port->OTYPER &= ~(1UL << pin);// type: output push pull (00 reset state)
    port->OSPEEDR |= (3UL << (pin*2)); // speed: High speed (11)
    port->PUPDR &= ~(3UL << (pin*2)); // resistor: no pull (00 reset state)
}

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

