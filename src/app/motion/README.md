# Motion & Homing (Units + MIN‑seek)

## Overview & Dependencies

**Modules:**

* `motion_units.c/.h` — axis configuration + conversions
* `home.c/.h` — single‑axis MIN homing sequence (blocking)

**Upstream dependencies:**

* `axis.h` — axis identifiers and direction mapping (`axis_cw_is_negative(a)`, etc.)
* `limits.h` — debounced MIN switch (`limits_init_min()`, `limits_poll_tick()`, `limits_min_pressed()`, `limits_block_neg()`)
* `stepgen_pwm_tim3.h` — stepper interface (`stepgen_enable/dir/move_n/busy`)
* `delay.h` — millisecond sleep used to pace polling during blocking waits

**Design intent:** keep the conversion math and homing policy *opinionated but minimal*, so it’s easy to extend into a fuller motion planner later.

---

## Units & Kinematics (`motion_units`)

### Axis configuration

```c
typedef struct {
    uint16_t full_steps_rev; // e.g., 200
    uint16_t microsteps;     // e.g., 8  → 200*8 = 1600 steps/rev
    float    mm_per_rev;     // e.g., 40.0 for belt/pulley, 8.0 for TR8×8 lead screw
} axis_cfg_t;
```

A private static table holds one `axis_cfg_t` per axis. The convenience init fills **defaults**:

* **X**: 200 steps/rev, 1/8 microstep, 40.0 mm/rev
* **Y**: 200 steps/rev, 1/8 microstep, 40.0 mm/rev
* **Z**: 200 steps/rev, 1/8 microstep, 8.0 mm/rev

> Update these to match *your* mechanics (pulley diameter/teeth, screw pitch, driver microstep mode).

### Derived quantities & conversions

* **Steps per mm**

  ```c
  float steps_per_mm(axis_t a);
  // = (full_steps_rev * microsteps) / mm_per_rev
  ```
* **Distance (mm) → Steps (uint32_t)**

  ```c
  uint32_t mm_to_steps(axis_t a, float mm);
  // rounds to nearest step: (steps_per_mm * mm + 0.5f)
  ```
* **Feed rate (mm/min) → Step frequency (Hz)**

  ```c
  uint32_t feed_to_hz(axis_t a, float feed_mm_min);
  // converts to mm/s, multiplies by steps_per_mm, rounds
  ```

**Rounding**: Both conversions round to the nearest whole step/Hz. This keeps long moves from accumulating fractional error and aligns nicely with a timer that expects integer ARR values.

---

## Homing (`home`)

### What it does

A conservative **MIN‑switch** homing cycle for one axis, fully blocking the caller until done (or a failure is detected):

1. **Safety back‑off** if we start with the MIN switch already pressed
2. **Fast seek** toward MIN (large span) — early stop occurs when the switch trips
3. **Back‑off** to release the switch
4. **Slow seek** toward MIN for precise edge latching
5. **Final clearance** to a small **home offset** so we end **un‑pressed**

It polls the debouncer every millisecond during motion to keep readings fresh.

### Parameters

```c
typedef struct {
    float fast_feed_mm_min; // fast seek speed (mm/min)
    float slow_feed_mm_min; // slow latch speed (mm/min)
    float backoff_mm;       // back‑off distance after a hit (mm)
    float seek_span_mm;     // max distance to look for the switch (mm)
    float home_offset_mm;   // where to park after success (>=0, usually a small clearance)
} home_params_t;
```

### Flow (simplified)

```c
home_init();                // seeds debouncers, settles a few ms
stepgen_enable(a, true);    // driver on (active‑LOW)
if (min_pressed) back_off();
fast_seek_toward_min();     // early‑stops on switch
if (!min_pressed) fail();
back_off_until_released();
slow_seek_toward_min();     // re‑latch precisely
if (!min_pressed) fail();
clear_to_home_offset();     // leave switch un‑pressed
return success;
```

### Helper behavior

* **Direction choice**: `set_dir_toward(axis, toward_negative)` maps intent into `stepgen_dir(axis, cw)` using `axis_cw_is_negative(axis)`.
* **Blocking move**: `move_mm_blocking(axis, mm, feed, toward_negative)` converts units to steps/Hz, commands `stepgen_move_n`, then loops on `stepgen_busy` while calling a 1 ms polling helper to refresh limit debouncing.
* **Debounce cadence**: `poll_1ms()` calls `limits_poll_tick()` and sleeps 1 ms (`delay(1)`), providing a ~1 kHz poll rate during waits.

### Return value

`bool home_axis_blocking(axis_t a, const home_params_t* p)`

* **true**: homing succeeded (MIN found and edge‑re‑latched, parked at offset)
* **false**: not found within span, stuck on switch, or unable to release between phases

> Note: Assigning the machine coordinate (e.g., `axis_set_machine_pos(a, 0.0)`) is **not** done here; do that in a higher layer after `true`.

---

## Public API

### motion_units.h

```c
void     motion_init_defaults(void);
float    steps_per_mm(axis_t a);
uint32_t mm_to_steps(axis_t a, float mm);
uint32_t feed_to_hz(axis_t a, float feed_mm_min);
```

### home.h

```c
void home_init(void);
bool home_axis_blocking(axis_t a, const home_params_t* p);
```

---

## Quick‑start Integration

```c
#include "motion_units.h"
#include "home.h"
#include "stepgen_pwm_tim3.h"

int main(void) {
    // ... clocks / BSP ...

    motion_init_defaults();
    stepgen_init_all();
    home_init();

    // Example homing for X
    const home_params_t XH = {
        .fast_feed_mm_min = 1200.0f, // tune to your mechanics
        .slow_feed_mm_min = 200.0f,
        .backoff_mm       = 3.0f,
        .seek_span_mm     = 200.0f,
        .home_offset_mm   = 1.0f,
    };

    if (!home_axis_blocking(AXIS_X, &XH)) {
        // handle failure (e.g., alarm)
    }

    // ... continue with Y, Z ... set machine coordinates, etc.
}
```

---

## Parameter Tuning Tips

* **fast_feed_mm_min**: as fast as your axis can reliably move without missing steps; use 30–60% of your comfortable max to leave margin.
* **slow_feed_mm_min**: 5–20% of fast feed for a clean latch point.
* **backoff_mm**: enough travel to **fully release** the switch (consider hysteresis), typically 2–5 mm.
* **seek_span_mm**: larger than the max expected distance from the current position to the MIN switch; if exceeded, the routine reports failure.
* **home_offset_mm**: small positive number (0.5–2 mm) to end un‑pressed; set to 0 for “on‑switch” homing (not recommended with mechanical switches).

---

## Safety Notes

* The low‑level **stepgen ISR** already aborts a move if MIN trips while moving negative. The homing layer relies on this and then **verifies** the state between phases.
* Ensure **EN polarity** matches your driver (assumed active‑LOW).
* Keep the **debounce poll rate** steady (~1 kHz) so transitions are recognized promptly.
* Consider integrating **E‑stop** checks at the application level during blocking waits if you expect global aborts during homing.

---

## Troubleshooting

* **Never finds the switch during fast seek**: Increase `seek_span_mm`; verify wiring and `*_ACTIVE_LOW` polarity; confirm the axis is actually moving negative (check `axis_cw_is_negative`).
* **Cannot release after back‑off**: Increase `backoff_mm`; verify switch travel and alignment; inspect for mechanical sticking.
* **Oscillates near the edge**: Increase debounce (more `DEBOUNCE_TICKS`) or reduce `slow_feed_mm_min`.
* **Ends still pressing the switch**: Increase `home_offset_mm`.


