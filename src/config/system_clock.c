#include "system_clock.h"


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

    // Enable HSEBypass, HSE (max 8Mhz crystal)
    RCC->CR |= RCC_CR_HSEBYP; 
    RCC->CR |= RCC_CR_HSEON;
    while(!(RCC->CR & RCC_CR_HSERDY)); // wait for HSE to be ready

    // Set the power enable clock and voltage regulator
    RCC->APB1ENR |= RCC_APB1ENR_PWREN; //power interface clock enable 
    PWR->CR |= PWR_CR_VOS; // VOS Regulator voltage scaling output: Scale mode 1 (reset value)
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
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE; // use HSE as PLL source
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLM_2; // 0x04 = 0100 // PLL_M = 4
    RCC->PLLCFGR |= (180 << RCC_PLLCFGR_PLLN_Pos); // PLL_N = 180
    RCC->PLLCFGR |= (0 << RCC_PLLCFGR_PLLP_Pos); // PLL_P = 2 (00b)

    // Enable the PLL
    RCC->CR |= RCC_CR_PLLON;
    while(!(RCC->CR & RCC_CR_PLLRDY)); // wait for PLL to lock

    // Configure the Prescalars HCLK, PCLK1, PCLK2 (AHB and APB prescalers)
    /*
    AHB Prescaler: /1
    APB1 Prescaler: /4
    APB2 Prescaler: /2
    */
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;   // AHB = SYSCLK / 1 = 180 MHz
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;  // APB1 = SYSCLK / 4 = 45 MHz
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;  // APB2 = SYSCLK / 2 = 90 MHz

    // Select system clock as PLL
    RCC->CFGR |= RCC_CFGR_SW_PLL; 
    while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL); // wait until PLL is system clock

    // Update global System Core Clock
    SystemCoreClock = 180000000;
    //pclk1_hz (APB1) 45 MHz
    //pclk2_hz (APB2) 90 MHz 
}

static uint32_t get_hclk_from_cfgr(void) {
    // We know you switch to PLL at 180 MHz and set AHB prescaler in CFGR,
    // so just decode AHB prescaler; SYSCLK is already 180 MHz.
    const uint32_t sysclk = 180000000UL;

    uint32_t hpre  = (RCC->CFGR >> 4) & 0xFUL;          // HPRE[7:4]
    uint32_t ahbdiv = (hpre < 8) ? 1UL : (1UL << (hpre - 7UL));
    return sysclk / ahbdiv;                             // = 180 MHz with your config
}

uint32_t System_GetPCLK1(void) {
    uint32_t hclk = get_hclk_from_cfgr();
    uint32_t ppre1 = (RCC->CFGR >> 10) & 0x7UL;         // PPRE1[12:10]
    uint32_t apb1div = (ppre1 < 4) ? 1UL : (1UL << (ppre1 - 3UL));
    return hclk / apb1div;                              // = 45 MHz with your config
}

uint32_t System_GetPCLK2(void) {
    uint32_t hclk = get_hclk_from_cfgr();
    uint32_t ppre2 = (RCC->CFGR >> 13) & 0x7UL;         // PPRE2[15:13]
    uint32_t apb2div = (ppre2 < 4) ? 1UL : (1UL << (ppre2 - 3UL));
    return hclk / apb2div;                              // = 90 MHz with your config
}

