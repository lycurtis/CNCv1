# cmake/arm-gcc-toolchain.cmake
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(TOOLCHAIN_PREFIX arm-none-eabi)

# find_program(CMAKE_C_COMPILER arm-none-eabi-gcc)
# find_program(CMAKE_ASM_COMPILER arm-none-eabi-gcc)

# find_program(CMAKE_OBJCOPY  arm-none-eabi-objcopy)
# find_program(CMAKE_SIZE     arm-none-eabi-size)

find_program(CMAKE_C_COMPILER   ${TOOLCHAIN_PREFIX}-gcc)
find_program(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++)
find_program(CMAKE_ASM_COMPILER ${TOOLCHAIN_PREFIX}-gcc)

find_program(CMAKE_OBJCOPY ${TOOLCHAIN_PREFIX}-objcopy)
find_program(CMAKE_SIZE    ${TOOLCHAIN_PREFIX}-size)
