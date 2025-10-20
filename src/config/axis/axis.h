#pragma once

#include <stdbool.h>
#include <stdint.h>

// Axes
typedef enum { AXIS_X = 0, AXIS_Y = 1, AXIS_Z = 2 } axis_t;

/* ---- Per-axis semantics (edit these to match your machine) ----
 * A) Does CW motion go toward the negative end (MIN)?
 */
#define AXIS_X_CW_IS_NEG 1
#define AXIS_Y_CW_IS_NEG 1
#define AXIS_Z_CW_IS_NEG 0

/* B) What DIR pin level means CW on each axis?
 * If DIR pin HIGH means CW, set =1; if LOW means CW, set =0.
 * (Start with 1,1,1 and flip if a motor runs opposite of CW.)
 */
#define AXIS_X_DIR_HIGH_IS_CW 1
#define AXIS_Y_DIR_HIGH_IS_CW 1
#define AXIS_Z_DIR_HIGH_IS_CW 1

static inline bool axis_cw_is_negative(axis_t a) {
    switch (a) {
    case AXIS_X:
        return AXIS_X_CW_IS_NEG;
    case AXIS_Y:
        return AXIS_Y_CW_IS_NEG;
    default:
        return AXIS_Z_CW_IS_NEG;
    }
}

static inline bool axis_dir_high_is_cw(axis_t a) {
    switch (a) {
    case AXIS_X:
        return AXIS_X_DIR_HIGH_IS_CW;
    case AXIS_Y:
        return AXIS_Y_DIR_HIGH_IS_CW;
    default:
        return AXIS_Z_DIR_HIGH_IS_CW;
    }
}