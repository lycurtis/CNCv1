#include "stm32f4xx.h"  // CMSIS device header
#include "system_clock.h"
#include "delay.h"
#include "stepgen_pwm_tim3.h"

extern void stepgen_init(void);
extern void stepgen_start(void);
extern void stepgen_stop(void);
extern void stepgen_set_hz(uint32_t hz);
extern void stepgen_dir(bool fwd);
extern void stepgen_enable(bool enable_low_active);