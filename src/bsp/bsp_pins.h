#pragma once

/*-------------- Occupied Pins -------------*/
/**
 * PA0 X_MIN (Limit Switch)
 * PA1 Y_MIN (Limit Switch)
 * PA2 USART2_TX
 * PA3 USART2_RX
 * PA4 Z_MIN (Limit Switch)
 * PA6 X_STEP
 * PA7 Y_STEP
 *
 * PB0 Z_STEP
 * PB4 X_DIR
 * PB5 Y_DIR
 * PB12 X_EN
 * PB13 Y_EN
 * PB14 Z_EN
 *
 * PC2 Z_DIR
 * PC13 ESTOP (USER Button/Switch)
 */

////////// Axes STEP/DIR/EN + IO //////////
// X-Axis
#define X_STEP_PORT GPIOA
#define X_STEP_PIN 6UL // PA6, e.g. TIM3_CH1
#define X_STEP_AF_VAL 2UL // AF2 -> TIM3_CH1

#define X_DIR_PORT GPIOB
#define X_DIR_PIN 4UL // PB4

#define X_EN_PORT GPIOB
#define X_EN_PIN 12UL // PB12 (note: LOW = enable)

#define X_MIN_PORT GPIOA
#define X_MIN_PIN 0UL // PA0 (X-axis Limit Switch)
#define X_MIN_ACTIVE_LOW 1 // 1 = pressed when pin reads 0

// Y-Axis
#define Y_STEP_PORT GPIOA
#define Y_STEP_PIN 7UL // PA7, e.g. TIM3_CH2
#define Y_STEP_AF_VAL 2UL // AF2 -> TIM3_CH2

#define Y_DIR_PORT GPIOB
#define Y_DIR_PIN 5UL // PB5

#define Y_EN_PORT GPIOB
#define Y_EN_PIN 13UL // PB13

#define Y_MIN_PORT GPIOA
#define Y_MIN_PIN 1UL // PA1 (Y-axis Limit Switch)
#define Y_MIN_ACTIVE_LOW 1 // 1 = pressed when pin reads 0

// Z-Axis
#define Z_STEP_PORT GPIOB
#define Z_STEP_PIN 0UL // PB0 TIM3_CH3
#define Z_STEP_AF_VAL 2UL // AF2 -> TIM3_CH2

#define Z_DIR_PORT GPIOC
#define Z_DIR_PIN 2UL // PC2

#define Z_EN_PORT GPIOB
#define Z_EN_PIN 14UL // PB14

#define Z_MIN_PORT GPIOA
#define Z_MIN_PIN 4UL // PA4 (Z-axis Limit Switch)
#define Z_MIN_ACTIVE_LOW 1 // 1 = pressed when pin reads 0

/*---- IO ----*/

// Emergency Stop (USER button)
#define ESTOP_PORT GPIOC
#define ESTOP_PIN 13UL // PC13 Emergency Switch (Default: Nucleo blue USER button)
#define ESTOP_ACTIVE_HIGH 0 // pressed = logic HIGH on Nucleo

// Debug UART2
#define DBG_TX_PORT GPIOA
#define DBG_TX_PIN 2UL // PA2 -> USART2_TX AF7

#define DBG_RX_PORT GPIOA
#define DBG_RX_PIN 3UL // PA3 -> USART3_RX AF7
