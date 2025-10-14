cmake_minimum_required(VERSION 3.22)
project(CNCv1 C ASM)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/toolchain-arm-none-eabi.cmake)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  # helps IntelliSense
set(CMAKE_VERBOSE_MAKEFILE ON) # Used to verify and check the compile lines to show -Og -g3 and no -flto. (debug mode)
#Verbose builds just make CMake show the exact compiler/linker commands it runs

# ---- Pick Debug by default (for single-config generators like Ninja/Make) ----
# if(NOT CMAKE_BUILD_TYPE)
#   set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
# endif()

# --- Target ---
add_executable(${PROJECT_NAME})

# --- Sources (list each .c/.s once) ---
target_sources(${PROJECT_NAME} PRIVATE
  src/app/main.c
  src/config/system_clock.c
  src/utils/delay.c
  src/drivers/stepgen_pwm_tim3.c
  src/bsp/bsp_gpio.c
  src/bsp/bsp_usart2_debug.c
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


# --- Compile options (per-config) ---
target_compile_options(${PROJECT_NAME} PRIVATE
  # common arch/warnings
  -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard
  -ffunction-sections -fdata-sections
  -Wall -Wextra

  # Debug: debug-friendly, no LTO, easy stepping
  $<$<CONFIG:Debug>:-Og -g3 -fno-omit-frame-pointer -fno-inline -fno-lto>

  # Release: optimized
  $<$<CONFIG:Release>:-O2>
)

# target_compile_options(${PROJECT_NAME} PRIVATE
#   -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard
#   -O2 -ffunction-sections -fdata-sections -Wall -Wextra
# )

# # ---- Debug-friendly flags; disable LTO ----
# target_compile_options(${PROJECT_NAME} PRIVATE
#   -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard
#   -Og -g3                         # <— instead of -O2
#   -fno-omit-frame-pointer
#   -fno-inline
#   -ffunction-sections -fdata-sections
#   -Wall -Wextra
#   -fno-lto                        # <— disable LTO (just in case)
# )

# --- Link options (per-config) ---
# target_link_options(${PROJECT_NAME} PRIVATE
#   -T${CMAKE_SOURCE_DIR}/mcu_support/st/STM32F446RETX_FLASH.ld
#   -Wl,--gc-sections -Wl,-Map=$<TARGET_FILE_DIR:${PROJECT_NAME}>/${PROJECT_NAME}.map
#   -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard
#   -fno-lto                        # <— disable LTO at link too
# )
target_link_options(${PROJECT_NAME} PRIVATE
  -T${CMAKE_SOURCE_DIR}/mcu_support/st/STM32F446RETX_FLASH.ld
  -Wl,--gc-sections -Wl,-Map=$<TARGET_FILE_DIR:${PROJECT_NAME}>/${PROJECT_NAME}.map
  -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard

  # Debug: disable LTO at link too
  $<$<CONFIG:Debug>:-fno-lto>
)

# --- Post-build artifacts ---
find_program(CMAKE_OBJCOPY arm-none-eabi-objcopy)
find_program(CMAKE_SIZE    arm-none-eabi-size)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_OBJCOPY} -O ihex   $<TARGET_FILE:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}>/${PROJECT_NAME}.hex
  COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}>/${PROJECT_NAME}.bin
  COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}>
)