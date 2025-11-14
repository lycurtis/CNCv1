# tests/CMakeLists.txt (separate from the firmware build)
cmake_minimum_required(VERSION 3.22)
project(CNCv1_tests C)

add_executable(test_motion_units
    test_motion_units.c
    ../src/app/motion/motion_units.c
)

# So the compiler can find headers used by motion_units.h
target_include_directories(test_motion_units PRIVATE
    ../src/app/motion
    ../src/drivers/stepgen
    ../mcu_support/Drivers/CMSIS/Include
    ../mcu_support/Drivers/CMSIS/Device/ST/STM32F4xx/Include
    ../src/config/axis
)

enable_testing()
add_test(NAME motion_units COMMAND test_motion_units)


# Note: This CMake file does not use STM32 toolchain file so that a normal host build with the PC’s compiler instead.