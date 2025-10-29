# Limit Switches (Debounced MIN)

## Overview

This module provides debounced readings for the **MIN** limit switch on each axis (X, Y, Z). It is intended to be polled at a fixed rate (e.g., **1 kHz**) from a periodic tick. The debounced states are then used by the motion layer (e.g., the step pulse generator ISR) to block unsafe motion.

**Key properties:**

* Per-axis **debouncer** using a short consecutive-sample counter
* Board-configurable **active-low/active-high** polarity
* **Input + pull-up** configuration for switches
* **Policy helper** `limits_block_neg(axis)` for motion layers

---

## Architecture & Data Flow

**Core types:**

* `Deb` – a per-line debouncer with three fields:

  * `cnt` – number of consecutive samples that differ from the current stable state
  * `stable` – the current **debounced** state (0/1)
  * `last_sample` – the previous **raw** sample (0/1)

**Static state:**

* `s_min_db[3]` – one `Deb` per axis (MIN only)

**Hardware map:**

* `LIM_MIN[3]` – per-axis `{port, pin, active_low}`

**Data flow each poll tick:**

1. Read the raw GPIO level (`IDR`) → convert to logical **active/not-active** via `active_low`
2. Feed the sample into the per-axis `deb_tick()`
3. `deb_tick()` updates `stable` only after **N** consecutive differing samples (debounce)

**Consumption:**

* Motion code calls `limits_min_pressed(axis)` or `limits_block_neg(axis)` at any time (ISR-safe reads of `stable`)

---

## Public API

Declared in `limits.h`:

```c
void limits_init_min(void);      // Configure X/Y/Z MIN pins (input + pull-up), seed debouncers
void limits_poll_tick(void);     // Call at a fixed rate (e.g., 1 kHz)
bool limits_min_pressed(axis_t); // Debounced MIN state per axis
bool limits_block_neg(axis_t);   // Policy: true if negative travel should be blocked
```

**Function details:**

* **`limits_init_min()`**

  * Enables GPIO clocks for any defined MIN pins
  * Sets pins as input with pull-up
  * Seeds each axis debouncer with the **current** logical sample to avoid a spurious edge on startup

* **`limits_poll_tick()`**

  * For each configured axis, reads the raw pin, applies polarity, and advances its debouncer via `deb_tick()`
  * Must be called at a **stable rate** (e.g., from SysTick, or a hardware timer ISR)

* **`limits_min_pressed(axis)`**

  * Returns the **debounced** press state (`stable != 0`)
  * No edge detection; it reflects the current filtered level

* **`limits_block_neg(axis)`**

  * Returns **true** when you should prevent motion **toward MIN**
  * In the reference motion ISR, this is consulted before decrementing steps when moving negative

---

## Lifecycle & Integration

1. **Initialization** (early in boot, after clocks):

   ```c
   limits_init_min();
   ```
2. **Periodic polling** at a fixed rate (e.g., 1 kHz):

   * **Option A (SysTick @ 1 kHz):**

     ```c
     void SysTick_Handler(void) {
         limits_poll_tick();
     }
     ```
   * **Option B (TIMx @ 1 kHz):**

     ```c
     void TIM14_IRQHandler(void) { // example
         if (TIM14->SR & TIM_SR_UIF) {
             TIM14->SR = ~TIM_SR_UIF;
             limits_poll_tick();
         }
     }
     ```
   * **Option C (main loop)** with a millisecond scheduler:

     ```c
     while (1) {
         if (millis_elapsed()) limits_poll_tick();
         // ... other work ...
     }
     ```
3. **Consumption in motion layer** (e.g., inside your TIM3 stepgen ISR):

   ```c
   if (moving_negative && limits_block_neg(axis)) {
       // disable channel, clear counter, etc.
   }
   ```

---

## Glossary

* **Debounce** — technique to ignore rapid on/off transitions (bounce) until input is stable
* **Active-low** — logical ON when the electrical signal is low
* **MIN/MAX** — negative/positive travel endstops
* **Poll rate** — frequency at which inputs are sampled and debounced
