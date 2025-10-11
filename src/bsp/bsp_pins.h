#pragma once


////////// Axes STEP/DIR/EN + IO //////////
// X-Axis
#define X_STEP_PORT GPIOA
#define X_STEP_PIN  6UL // PA6, e.g. TIM3_CH1
#define X_STEP_AF_VAL 2UL // AF2 -> TIM3_CH1

#define X_DIR_PORT  GPIOB
#define X_DIR_PIN   4UL // PB4

#define X_EN_PORT GPIOB
#define X_EN_PIN 12UL // PB12 (note: LOW = enable)

// Y-Axis
#define Y_STEP_PORT GPIOA
#define Y_STEP_PIN  7UL // PA7, e.g. TIM3_CH2
#define Y_STEP_AF_VAL 2UL // AF2 -> TIM3_CH2

#define Y_DIR_PORT GPIOB
#define Y_DIR_PIN 5UL // PB5

#define Y_EN_PORT GPIOB
#define Y_EN_PIN 13UL // PB13

// Z-Axis
#define Z_STEP_PORT GPIOB
#define Z_STEP_PIN  0UL // TIM3_CH3
#define Z_STEP_AF_VAL 2UL // AF2 -> TIM3_CH2

#define Z_DIR_PORT GPIOC
#define Z_DIR_PIN 2UL // PC2

#define Z_EN_PORT GPIOB
#define Z_EN_PIN 14UL // PB14

////////// IO //////////
// Limit switches / E-Stop
// #define X_MIN_PORT  GPIOC
// #define X_MIN_PIN   (1U<<13)
// #define ESTOP_PORT  GPIOB
// #define ESTOP_PIN   (1U<<2)

// // Debug UART
// #define DBG_UART USART2
// #define DBG_TX_PORT GPIOA
// #define DBG_TX_PIN  (1U<<2) // PA2 AF7
// #define DBG_RX_PORT GPIOA
// #define DBG_RX_PIN  (1U<<3) // PA3 AF7
