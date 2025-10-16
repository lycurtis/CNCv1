#include "system_clock.h"

#include "stm32f4xx.h"

void system_clock_init(void){
    /**
    * @brief  System Clock Configuration
    *         The system Clock is configured as follow : 
    *            System Clock source            = PLL (HSE)
    *            SYSCLK(Hz)                     = 180000000
    *            HCLK(Hz)                       = 180000000
    *            AHB Prescaler                  = 1
    *            APB1 Prescaler                 = 4
    *            APB2 Prescaler                 = 2
    *            HSE Frequency(Hz)              = 8000000
    *            PLL_M                          = 4
    *            PLL_N                          = 180
    *            PLL_P                          = 2
    *            VDD(V)                         = 3.3
    *            Main regulator output voltage  = Scale1 mode
    *            Flash Latency(WS)              = 5
    * @param  None
    * @retval None
    */

    // HSE Bypass Mode: 
    /*In this mode, an external clock source must be provided. You select this mode by setting the
    HSEBYP and HSEON bits in the RCC clock control register (RCC_CR)*/

    // Set the power enable clock and voltage regulator
    RCC->APB1ENR |= RCC_APB1ENR_PWREN; //power interface clock enable 
    PWR->CR |= PWR_CR_VOS; // VOS Regulator voltage scaling output: Scale mode 1 (reset value)

    // Enable HSEBypass, HSE (max 8Mhz crystal)
    RCC->CR |= RCC_CR_HSEBYP; 
    RCC->CR |= RCC_CR_HSEON;
    while(!(RCC->CR & RCC_CR_HSERDY)); // wait for HSE to be ready

    
    // Enable overdrive mode 
    PWR->CR |= PWR_CR_ODEN; // Achieve higher frequency with overdrive mode

    // Configure Flash: 5 wait states, prefetch, instruction and data cache
    FLASH->ACR |= FLASH_ACR_LATENCY_5WS | FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN;

    // Configure the main PLL
    // PLL_VCO = (HSE / M) * N = (8 / 4) * 180 = 360 MHz
    // SYSCLK = PLL_VCO / P = 360 / 2 = 180 MHz
    /*
    PLL Source: HSE
    PLLM: /4
    PLLN: x180
    PLLP: /2
    */
    RCC->PLLCFGR = 0; // start from scratch
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE
                 |  (4U   << RCC_PLLCFGR_PLLM_Pos)   // M
                 |  (180U << RCC_PLLCFGR_PLLN_Pos)   // N
                 |  (0U   << RCC_PLLCFGR_PLLP_Pos);  // P=/2 (00b)


    // Enable the PLL
    RCC->CR |= RCC_CR_PLLON;
    while(!(RCC->CR & RCC_CR_PLLRDY)); // wait for PLL to lock

    // Configure the Prescalars HCLK, PCLK1, PCLK2 (AHB and APB prescalers)
    /*
    AHB Prescaler: /1
    APB1 Prescaler: /4
    APB2 Prescaler: /2
    */
    RCC->CFGR &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2);
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;   // AHB = SYSCLK / 1 = 180 MHz
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;  // APB1 = SYSCLK / 4 = 45 MHz
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;  // APB2 = SYSCLK / 2 = 90 MHz

    // Select system clock as PLL
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_PLL;
    while(((RCC->CFGR & RCC_CFGR_SWS) >> RCC_CFGR_SWS_Pos) != 0b10) { }

    SystemCoreClock = 180000000U; // keep this consistent with reality
    //pclk1_hz (APB1) 45 MHz
    //pclk2_hz (APB2) 90 MHz 
}

// DWT (Data Watch Point Trigger)
void dwt_enable(void){
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // enable trace subsystem (must be set or writes to DWT registers will be ignored)
    DWT->CYCCNT = 0; // reset cycle counter (32 bit counter that increments every CPU clock tick)
    DWT->CTRL  |= DWT_CTRL_CYCCNTENA_Msk; // start counting
}

static inline uint32_t dwt_cycles(void){ // reads the current cycle count
    return DWT->CYCCNT; 
}

// --- TIM2: 10 ms one-shot, assuming TIM2CLK = PCLK1*2 = 90 MHz ---
static void tim2_setup_10ms(void){
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    TIM2->CR1 = 0; // clear 
    TIM2->CR1 |= TIM_CR1_OPM; // one-pulse mode: counter stops counting at the next update event (clearing the bit CEN)

    TIM2->PSC = 8999; // Prescaler: The counter clock frequency (CK_CNT) is equal to fCK_PSC / (PSC[15:0] + 1) ==> 90 MHz / (8999+1) = 10 kHz 
    TIM2->ARR = 99; // Auto Reload: period = (ARR+1) ticks ==> (99+1) = 100 ticks @10 kHz => 10.000 ms
    TIM2->EGR = TIM_EGR_UG; // load PSC/ARR
    TIM2->SR  = 0; // clear flags
}

static void tim2_start_and_wait(void){
    TIM2->CNT = 0; // start count from 0
    TIM2->SR  = 0; // 
    TIM2->CR1 |= TIM_CR1_CEN; // start counting
    while((TIM2->SR & TIM_SR_UIF) == 0); // spins until UIF (update interrupt flag) sets an overflow (i.e. after 10ms)
}

uint32_t measure_10ms_cycles(void){ // Expect ~1,800,000. Core Hz ≈ cycles_10ms * 100.
    dwt_enable();
    tim2_setup_10ms();

    // optional warm-up run
    tim2_start_and_wait();

    uint32_t c0 = dwt_cycles();
    tim2_start_and_wait(); // exactly one 10 ms period
    uint32_t c1 = dwt_cycles();

    return (c1 - c0); // ~1,800,000 if core = 180 MHz
}