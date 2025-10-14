# cmake/arm-gcc-toolchain.cmake
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY) # Avoid try-compile running a host linker

# set(TOOLCHAIN_PREFIX arm-none-eabi)

# find_program(CMAKE_C_COMPILER arm-none-eabi-gcc)
# find_program(CMAKE_ASM_COMPILER arm-none-eabi-gcc)

# find_program(CMAKE_OBJCOPY  arm-none-eabi-objcopy)
# find_program(CMAKE_SIZE     arm-none-eabi-size)

# find_program(CMAKE_C_COMPILER   ${TOOLCHAIN_PREFIX}-gcc)
# find_program(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++)
# find_program(CMAKE_ASM_COMPILER ${TOOLCHAIN_PREFIX}-gcc)

# find_program(CMAKE_OBJCOPY ${TOOLCHAIN_PREFIX}-objcopy)
# find_program(CMAKE_SIZE    ${TOOLCHAIN_PREFIX}-size)

# Point to your desired GNU Arm toolchain root
set(TOOLCHAIN_DIR "C:/Program Files (x86)/GNU Arm Embedded Toolchain/10 2021.10")

# Explicit compilers
set(CMAKE_C_COMPILER   "${TOOLCHAIN_DIR}/bin/arm-none-eabi-gcc.exe")
set(CMAKE_CXX_COMPILER "${TOOLCHAIN_DIR}/bin/arm-none-eabi-g++.exe")
set(CMAKE_ASM_COMPILER "${TOOLCHAIN_DIR}/bin/arm-none-eabi-gcc.exe")

# (optional) binutils used later
set(CMAKE_AR        "${TOOLCHAIN_DIR}/bin/arm-none-eabi-ar.exe")
set(CMAKE_OBJCOPY   "${TOOLCHAIN_DIR}/bin/arm-none-eabi-objcopy.exe")
set(CMAKE_SIZE      "${TOOLCHAIN_DIR}/bin/arm-none-eabi-size.exe")