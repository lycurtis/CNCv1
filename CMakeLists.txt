cmake_minimum_required(VERSION 3.22)
project(CNCv1 C ASM)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/toolchain-arm-none-eabi.cmake)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  # helps IntelliSense

# --- Target ---
add_executable(${PROJECT_NAME})

# --- Sources (list each .c/.s once) ---
target_sources(${PROJECT_NAME} PRIVATE
  src/app/main.c
  src/config/system_clock.c
  src/utils/delay.c
  src/drivers/stepgen_pwm_tim3.c
  src/bsp/bsp_gpio.c
  mcu_support/st/system_stm32f4xx.c
  mcu_support/st/Startup/startup_stm32f446xx.s
)

# --- Includes ---
target_include_directories(${PROJECT_NAME} PRIVATE
  src/app
  src/bsp
  src/config
  src/drivers
  src/utils
  mcu_support/Drivers/CMSIS/Include
  mcu_support/Drivers/CMSIS/Device/ST/STM32F4xx/Include
)

# --- Defines/flags/ld ---
target_compile_definitions(${PROJECT_NAME} PRIVATE STM32F446xx)
target_compile_options(${PROJECT_NAME} PRIVATE
  -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard
  -O2 -ffunction-sections -fdata-sections -Wall -Wextra
)
target_link_options(${PROJECT_NAME} PRIVATE
  -T${CMAKE_SOURCE_DIR}/mcu_support/st/STM32F446RETX_FLASH.ld
  -Wl,--gc-sections -Wl,-Map=$<TARGET_FILE_DIR:${PROJECT_NAME}>/${PROJECT_NAME}.map
  -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard
)

# --- Post-build artifacts ---
find_program(CMAKE_OBJCOPY arm-none-eabi-objcopy)
find_program(CMAKE_SIZE    arm-none-eabi-size)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_OBJCOPY} -O ihex   $<TARGET_FILE:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}>/${PROJECT_NAME}.hex
  COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}>/${PROJECT_NAME}.bin
  COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}>
)
