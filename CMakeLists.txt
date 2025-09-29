cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
# set(CMAKE_C_STANDARD 11)
# set(CMAKE_C_STANDARD_REQUIRED ON)
# set(CMAKE_C_EXTENSIONS ON)


# Define the build type
# if(NOT CMAKE_BUILD_TYPE)
#     set(CMAKE_BUILD_TYPE "Debug")
# endif()

# Set the project name
# set(CMAKE_PROJECT_NAME sandbox)

# Enable compile command to ease indexing with e.g. clangd
# set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
# project(${CMAKE_PROJECT_NAME})
# message("Build type: " ${CMAKE_BUILD_TYPE})
project(CNCv1 C ASM)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/toolchain-arm-none-eabi.cmake)

# Enable CMake support for ASM and C languages
# enable_language(C ASM)

# Create an executable object type
# add_executable(${CMAKE_PROJECT_NAME})
add_executable(${PROJECT_NAME}
  src/main.c
  vendor/system_stm32f4xx.c
  vendor/Startup/startup_stm32f446xx.s
)


add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_OBJCOPY} -O ihex   $<TARGET_FILE:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}>/${PROJECT_NAME}.hex
  COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}>/${PROJECT_NAME}.bin
)

# Add STM32CubeMX generated sources
# add_subdirectory(cmake/stm32cubemx)

# Link directories setup
# target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
#     # Add user defined library search paths
# )

# Add sources to executable
# target_sources(${CMAKE_PROJECT_NAME} PRIVATE
#     # Add user sources here
# )

# Add include paths
# target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
#     # Add user defined include paths
# )
target_include_directories(${PROJECT_NAME} PRIVATE
  vendor/Drivers/CMSIS/Include
  vendor/Drivers/CMSIS/Device/ST/STM32F4xx/Include
)

# Add project symbols (macros)
# target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
#     # Add user defined symbols
# )
target_compile_definitions(${PROJECT_NAME} PRIVATE STM32F446xx)
target_compile_options(${PROJECT_NAME} PRIVATE
  -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard
  -O2 -ffunction-sections -fdata-sections -Wall -Wextra
)
target_link_options(${PROJECT_NAME} PRIVATE
  -T${CMAKE_SOURCE_DIR}/vendor/STM32F446RETX_FLASH.ld
  -Wl,--gc-sections -Wl,-Map=$<TARGET_FILE_DIR:${PROJECT_NAME}>/${PROJECT_NAME}.map
  -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard
)

# Remove wrong libob.a library dependency when using cpp files
# list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
# target_link_libraries(${CMAKE_PROJECT_NAME}
#     stm32cubemx

#     # Add user defined libraries
# )
# Optional: print size after link
find_program(SIZE arm-none-eabi-size)
if (SIZE)
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${SIZE} $<TARGET_FILE:${PROJECT_NAME}>
  )
endif()