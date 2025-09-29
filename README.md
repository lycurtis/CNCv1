# CNCv1 – STM32 Bare-Metal + CMake + VS Code

A minimal STM32 bare-metal project template using **CMake**, **Ninja**, **VS Code**, and **OpenOCD**.  
Designed for STM32F4 (tested on **STM32F446RE Nucleo**) but easily adaptable to other MCUs.

---

## Features
- Bare-metal (no HAL, no LL)
- CMake + Ninja build system
- Works in VS Code with CMake Tools + Cortex-Debug
- OpenOCD for flashing & debugging
- Minimal project structure (startup, linker, CMSIS only)

---

## Prerequisites

Install these tools before building:

1. **Arm GNU Toolchain**  
   [Download](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)  
   ```sh
   arm-none-eabi-gcc --version

1. **CMake (>=3.22)**  
   [Download](https://cmake.org/download/)  
   ```sh
   cmake --version

1. **Ninja Build System**  
   [Download](https://github.com/ninja-build/ninja)  
   ```sh
   ninja --version

1. **OpenOCD (for flashing/deubgging)**  
   [Download](https://gnutoolchains.com/arm-eabi/openocd/)  
   ```sh
   openocd --version