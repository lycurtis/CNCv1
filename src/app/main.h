#include "stm32f4xx.h"  // CMSIS device header
#include "system_clock.h"
#include "delay.h"
#include "stepgen_pwm_tim3.h"


extern void stepgen_init_all(void);
extern void stepgen_start_all(void);
extern void stepgen_stop_all(void);

extern void stepgen_enable(axis_t a, bool enable_outputs_low_active);
extern void stepgen_dir(axis_t a, bool fwd);
extern void stepgen_set_hz(axis_t a, uint32_t hz);
