# cmake/arm-gcc-toolchain.cmake
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY) # Avoid try-compile running a host linker

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

