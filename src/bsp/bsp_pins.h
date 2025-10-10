#pragma once

// Axes STEP/DIR/EN + IO
#define X_STEP_PORT GPIOA
#define X_STEP_PIN  6UL // PA6, e.g. TIM3_CH1
#define X_DIR_PORT  GPIOB
#define X_DIR_PIN   0UL // PB0
#define X_EN_PORT GPIOB
#define X_EN_PIN 1UL // PB1 (note: LOW = enable)

#define Y_STEP_PORT GPIOA
#define Y_STEP_PIN  (1U<<7) // TIM3_CH2
#define Z_STEP_PORT GPIOB
#define Z_STEP_PIN  (1U<<0) // TIM3_CH3/CH4 if you change mapping

// Limit switches / E-Stop
#define X_MIN_PORT  GPIOC
#define X_MIN_PIN   (1U<<13)
#define ESTOP_PORT  GPIOB
#define ESTOP_PIN   (1U<<2)

// Debug UART
#define DBG_UART USART2
#define DBG_TX_PORT GPIOA
#define DBG_TX_PIN  (1U<<2) // PA2 AF7
#define DBG_RX_PORT GPIOA
#define DBG_RX_PIN  (1U<<3) // PA3 AF7
