# CMakeLists.txt

cmake_minimum_required(VERSION 3.22)

project(CNCv1 C ASM)

# Toolchain + quality of life
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/toolchain-arm-none-eabi.cmake)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  # helps IntelliSense
set(CMAKE_VERBOSE_MAKEFILE ON) # Used to verify and check the compile lines to show -Og -g3 and no -flto. (debug mode)
#Verbose builds just make CMake show the exact compiler/linker commands it runs

# ---- Pick Debug by default (for single-config generators like Ninja/Make) ----
# if(NOT CMAKE_BUILD_TYPE)
#   set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
# endif()

# ---- Usage requirements shared by all code (flags/defines) ----
add_library(fw_opts INTERFACE)

# target_include_directories(fw_opts INTERFACE
#   ${CMAKE_SOURCE_DIR}/src/app
#   ${CMAKE_SOURCE_DIR}/src/bsp
#   ${CMAKE_SOURCE_DIR}/src/config
#   ${CMAKE_SOURCE_DIR}/src/drivers
#   ${CMAKE_SOURCE_DIR}/src/utils
# )

target_compile_definitions(fw_opts INTERFACE STM32F446xx)
# --- Compile options (per-config) ---
target_compile_options(fw_opts INTERFACE
  # common arch/warnings
  -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard
  -ffunction-sections -fdata-sections
  -Wall -Wextra

  # Debug: debug-friendly, no LTO, easy stepping
  $<$<CONFIG:Debug>:-Og -g3 -fno-omit-frame-pointer -fno-inline -fno-lto>

  # Release: optimized
  $<$<CONFIG:Release>:-O2>
)

# ---- Bring in vendor/CMSIS and startup ----
add_subdirectory(mcu_support)

# ---- Your source tree (app/bsp/drivers/config/utils) ----
add_subdirectory(src)

# ---- Final firmware image ----
add_executable(${PROJECT_NAME}
  $<TARGET_OBJECTS:stm32_startup>  # startup + system

  # modules (bring in main.c and the rest)
  $<TARGET_OBJECTS:app_obj>
  $<TARGET_OBJECTS:bsp_obj>
  $<TARGET_OBJECTS:config_obj>
  $<TARGET_OBJECTS:drivers_obj>
  $<TARGET_OBJECTS:utils_obj>
)

# Link usage requirements (#include paths, flags)
target_link_libraries(${PROJECT_NAME} PRIVATE fw_opts cmsis_headers)

# Linker script + map + no dead code
# Linker script + map + no dead code
target_link_options(${PROJECT_NAME} PRIVATE
  -T${CMAKE_SOURCE_DIR}/mcu_support/st/STM32F446RETX_FLASH.ld
  -Wl,--gc-sections
  -Wl,-Map=$<TARGET_FILE_DIR:${PROJECT_NAME}>/${PROJECT_NAME}.map
  -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard
  $<$<CONFIG:Debug>:-fno-lto>
)

# Post-build: hex/bin/size
find_program(CMAKE_OBJCOPY arm-none-eabi-objcopy)
find_program(CMAKE_SIZE    arm-none-eabi-size)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_OBJCOPY} -O ihex   $<TARGET_FILE:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}>/${PROJECT_NAME}.hex
  COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}>/${PROJECT_NAME}.bin
  COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}>
)