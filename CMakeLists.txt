cmake_minimum_required(VERSION 3.22)

# Prefer passing the toolchain via CMakePresets.json; this keeps it safe if not set.
if(NOT CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE
      "${CMAKE_SOURCE_DIR}/cmake/toolchain-arm-none-eabi.cmake"
      CACHE FILEPATH "ARM GCC toolchain file")
endif()

project(CNCv1 C ASM)

# QoL
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

# ---- Common flags/defines shared by all code ----
add_library(fw_opts INTERFACE)
target_compile_definitions(fw_opts INTERFACE STM32F446xx)
target_compile_options(fw_opts INTERFACE
  -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard
  -ffunction-sections -fdata-sections
  -Wall -Wextra
  $<$<CONFIG:Debug>:-Og -g3 -fno-omit-frame-pointer -fno-inline -fno-lto>
  $<$<CONFIG:Release>:-O2>
)

# ---- CMSIS headers + startup (OBJECT) ----
add_subdirectory(mcu_support)  # defines: cmsis_headers (INTERFACE), stm32_startup (OBJECT)

# ---- Project code as STATIC libs (pro-grade layout lives under src/*) ----
add_subdirectory(src)  # defines: motion, stepgen, bsp, cfg, utils (STATIC)

# ---- Final firmware image ----
add_executable(${PROJECT_NAME}
  $<TARGET_OBJECTS:stm32_startup>     # from mcu_support
  ${CMAKE_SOURCE_DIR}/src/app/main.c  # keep main in the executable
)

# Link usage requirements and feature libs
target_link_libraries(${PROJECT_NAME} PRIVATE
  motion
  stepgen
  bsp
  cfg
  utils
  fw_opts
  cmsis_headers
)

# Linker script + map + dead-code removal + arch flags
target_link_options(${PROJECT_NAME} PRIVATE
  -T${CMAKE_SOURCE_DIR}/mcu_support/st/STM32F446RETX_FLASH.ld
  -Wl,--gc-sections
  -Wl,-Map=$<TARGET_FILE_DIR:${PROJECT_NAME}>/${PROJECT_NAME}.map
  -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard
  $<$<CONFIG:Debug>:-fno-lto>
)

# ---- Post-build artifacts ----
find_program(CMAKE_OBJCOPY arm-none-eabi-objcopy)
find_program(CMAKE_SIZE    arm-none-eabi-size)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_OBJCOPY} -O ihex   $<TARGET_FILE:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}>/${PROJECT_NAME}.hex
  COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}>/${PROJECT_NAME}.bin
  COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}>
)
