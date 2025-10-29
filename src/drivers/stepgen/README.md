# Step Pulse Generator (TIM3)

---

## Overview

This module produces precise **STEP** pulses on TIM3 PWM channels while exposing simple functions to:

* Enable/disable a stepper driver (active‑LOW EN)
* Set direction (DIR)
* Run **N steps at F Hz**
* Poll whether an axis is still moving

The implementation uses **preload registers** for glitch‑free ARR/CCR updates and a **1 MHz timer tick** (1 µs resolution) to make step‑rate math trivial. Each timer **update event** corresponds to **one step** on any enabled axis.

**Scope:** Coordinated multi‑axis moves at a **shared step rate** are supported (all active channels use the same ARR). Per‑axis rates require extensions (see *Future Extensions*).

---

## Key Features

* Up to **3 axes** on **TIM3 CH1/CH2/CH3**
* **1 MHz** timer base (microsecond granularity)
* **50% duty** STEP pulses (clean timing for most drivers)
* **Active‑LOW ENABLE** semantics (TMC2209‑friendly)
* **E‑stop** hard abort from the ISR
* **Negative‑limit block** (refuse motion toward MIN when tripped)
* Minimal CPU load (hardware PWM; ISR only counts down steps / enforces safety)

---

## Architecture

**MCU assumptions** (default clock tree):

* `SYSCLK = 180 MHz`
* `APB1 prescaler = 4` → `APB1 = 45 MHz`
* STM32 timer rule: if APB prescaler > 1, **timer clock = 2×APB** → `TIM3 = 90 MHz`
* **Prescaler PSC = 89** → `CK_CNT = 90 MHz / (89 + 1) = 1 MHz`

**Channel mapping (from `bsp_pins.h`):**

```
// X
X_STEP_PORT, X_STEP_PIN, X_STEP_AF_VAL  → TIM3_CH1
X_DIR_PORT,  X_DIR_PIN
X_EN_PORT,   X_EN_PIN   (active‑LOW)

// Y → TIM3_CH2
Y_STEP_PORT, Y_STEP_PIN, Y_STEP_AF_VAL
Y_DIR_PORT,  Y_DIR_PIN
Y_EN_PORT,   Y_EN_PIN

// Z → TIM3_CH3
Z_STEP_PORT, Z_STEP_PIN, Z_STEP_AF_VAL
Z_DIR_PORT,  Z_DIR_PIN
Z_EN_PORT,   Z_EN_PIN
```

> Configure those macros in your board pin header. STEP pins must be on the correct **AF** for TIM3.

**Module dependencies:**

* `axis.h` — defines `axis_t` (e.g., `AXIS_X, AXIS_Y, AXIS_Z`) and helpers like `axis_dir_high_is_cw(a)` and `axis_cw_is_negative(a)`
* `bsp_gpio.h`, `bsp_pins.h` — board‑support GPIO helpers and pin macros above
* `estop.h` — `estop_latched()`
* `limits.h` — `limits_block_neg(axis)` for MIN (negative) hard‑limit

**State kept per axis:**

* `steps_remaining[3]` — countdown for in‑flight moves
* `dir_is_cw[3]` — remembers last commanded CW/CCW
* `moving_mask` — bitfield of axes currently stepping (bit0=X, bit1=Y, bit2=Z)

**How it ticks:**

* **TIM3 PWM** drives STEP; DIR/EN are plain GPIO
* On each **update event** (end of PWM period = one step), the ISR:

  1. Aborts all motion if e‑stop is latched
  2. For each axis: blocks unsafe negative motion if MIN limit is asserted
  3. Decrements `steps_remaining`; disables a channel when it hits 0
  4. Stops the timer when no axes are moving

---

## Timing & Math

With a **1 MHz** counter clock:

* Desired step rate `Hz` → `ARR = (1_000_000 / Hz) - 1`
* 50% duty → `CCR = (ARR + 1) / 2`
* Example: `Hz = 1000` → `ARR = 999`, `CCR = 500` → 1 kHz STEP, 500 µs high, 500 µs low

**Important:** ARR is **global** per timer → all active channels run at the **same frequency**.

---

## Public API

Declared in `stepgen_pwm_tim3.h`:

```c
void stepgen_init_all(void);
void stepgen_start_all(void);
void stepgen_stop_all(void);

void stepgen_enable(axis_t a, bool enable_outputs_low_active);
void stepgen_dir(axis_t a, bool fwd);
void stepgen_set_hz(axis_t a, uint32_t hz);

void stepgen_move_n(axis_t a, uint32_t steps, uint32_t hz);
bool stepgen_busy(axis_t a); // true while that axis is mid‑move
```

### Function details

* **`stepgen_init_all()`** — Enables TIM3 clock + NVIC, sets PSC=89 (1 MHz), ARR=999 (1 kHz default), enables preload (ARPE), configures GPIO and PWM mode for CH1/2/3, initializes CCR=50% and latches via EGR UG.
* **`stepgen_start_all()` / `stepgen_stop_all()`** — Sets/clears `TIM3->CR1.CEN`. Auto‑stopped by ISR when no axes are moving.
* **`stepgen_enable(a, true)`** — Drives EN **LOW** (active‑LOW) to power the driver; `false` drives EN HIGH (disable). Uses atomic BSRR writes.
* **`stepgen_dir(a, fwd)`** — Sets DIR pin based on your board mapping `axis_dir_high_is_cw(a)` and records CW/CCW for later limit logic.
* **`stepgen_set_hz(a, hz)`** — Computes and preloads ARR from 1 MHz tick and sets that axis CCR to 50%. `hz==0` stops the timer. Uses **EGR UG** to latch ARR/CCR synchronously.
* **`stepgen_move_n(a, steps, hz)`** — Ignores no‑ops (`steps==0 || hz==0`) and e‑stop; blocks if the move would go **toward MIN** while the MIN switch is asserted; otherwise sets frequency, loads the axis step counter, enables that PWM channel, sets the `moving_mask` bit, and starts TIM3.
* **`stepgen_busy(a)`** — Returns whether `a` is still in the `moving_mask`.

---

## Troubleshooting

* **No motion:**

  * Ensure **EN is LOW** (active) before commanding motion.
  * Verify STEP pin AF is correct and the channel is **enabled** (CCxE set).
  * Confirm `hz > 0` and `steps > 0` in your call.
* **Moves ignored when going negative:** MIN switch is likely asserted; the code intentionally blocks motion toward MIN while tripped.
* **Wrong direction:** Check `axis_dir_high_is_cw(a)` mapping and your wiring. Swap coils only as a last resort.
* **Jitter or wrong speed:** If you changed clocks, make sure PSC still yields **1 MHz**. Confirm `ARR` math.
* **ISR never fires:** NVIC not enabled, or timer `CEN` is off. `stepgen_init_all()` enables UIE and NVIC—make sure you called it.

---

## Validation & Test Ideas

* **Logic analyzer / scope:** Probe STEP to verify frequency and 50% duty (e.g., 1 kHz → 1.000 ms period).
* **Limit test:** Hold the MIN switch active and attempt a negative move → it should be ignored. Positive moves should still proceed.
* **E‑stop test:** Trigger e‑stop mid‑move and confirm channels disable immediately and the timer stops.
* **Long move soak:** Run 100k+ steps and confirm no drift or missed steps (check with an index marker on the leadscrew/belt).

---

## Future Extensions

* **Independent per‑axis rates:**

  * Assign different axes to different timers, *or*
  * Implement a DDA/Bresenham step scheduler that toggles CCRs at per‑axis intervals on a single high‑rate base timer
* **Acceleration profiles:** Trapezoidal or S‑curve planner (update ARR over time via preloads)
* **Homing routine:** Integrate seek/back‑off using the existing MIN block logic
* **Max‑side (positive) limit support**
* **Jerk‑limited motion**

---

## Glossary

* **ARR** — Auto‑Reload Register (period)
* **CCR** — Capture/Compare Register (duty)
* **PSC** — Prescaler (divides timer input clock)
* **UIE/UIF** — Update Interrupt Enable/Flag (period elapse)
* **ARPE** — Auto‑reload preload enable (glitch‑free ARR updates)
* **EGR UG** — Event Generation: Update (latch ARR/CCR, reset CNT)

