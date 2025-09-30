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
- Ensure to add each <install>\bin to PATH

1. **Arm GNU Toolchain**  
   [Download](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)  
   ```sh
   arm-none-eabi-gcc --version

2. **CMake (>=3.22)**  
   [Download](https://cmake.org/download/)  
   ```sh
   cmake --version

3. **Ninja Build System**  
   [Download](https://github.com/ninja-build/ninja)  
   ```sh
   ninja --version

4. **OpenOCD (for flashing/deubgging)**  
   [Download](https://gnutoolchains.com/arm-eabi/openocd/)  
   ```sh
   openocd --version

5. **STM32CubeProgrammer (for ST-LINK drivers + optional flashing GUI)**  
   [Download](https://www.st.com/en/development-tools/stm32cubeprog.html)  
   - Confirm ST-LINK shows up in Device Manager without erros

6. **VS Code + Extensions**  
   - CMake Tools
   - Cortex-Debug
   - C/C++

## Project Structure 
```
tree /f > project_tree.txt
```
```
Firmware/
├─ CMakeLists.txt
├─ cmake/
│  └─ toolchain-arm-none-eabi.cmake
├─ src/
│  └─ main.c
├─ vendor/
│  ├─ Drivers/
│  │  ├─ CMSIS/Include/                      (core_cm4.h, cmsis headers)
│  │  └─ CMSIS/Device/ST/STM32F4xx/Include/  (stm32f4xx.h, stm32f446xx.h, system_stm32f4xx.h)
│  ├─ Startup/startup_stm32f446xx.s
│  ├─ STM32F446RETX_FLASH.ld
│  └─ system_stm32f4xx.c
└─ .vscode/
   ├─ settings.json
   ├─ tasks.json
   └─ launch.json
```

## Toolchain File (cmake/toolchain-arm-none-eabi.cmake)
```
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

find_program(CMAKE_C_COMPILER arm-none-eabi-gcc)
find_program(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
find_program(CMAKE_OBJCOPY arm-none-eabi-objcopy)
find_program(CMAKE_SIZE arm-none-eabi-size)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
```

## CMakeLists.txt (minimal)
```
cmake_minimum_required(VERSION 3.20)
project(CNCv1 C ASM)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/toolchain-arm-none-eabi.cmake)

add_executable(${PROJECT_NAME}
  src/main.c
  vendor/system_stm32f4xx.c
  vendor/Startup/startup_stm32f446xx.s
)

target_include_directories(${PROJECT_NAME} PRIVATE
  vendor/Drivers/CMSIS/Include
  vendor/Drivers/CMSIS/Device/ST/STM32F4xx/Include
)

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

# Generate .hex and .bin automatically
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_OBJCOPY} -O ihex   $<TARGET_FILE:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}>/${PROJECT_NAME}.hex
  COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}>/${PROJECT_NAME}.bin
)
```
## Typical Workflow
1. First Configure ( Run once or when toolchain/build system changes):
```
$tc = (Resolve-Path .\cmake\toolchain-arm-none-eabi.cmake).Path
cmake -S . -B build -G Ninja "-DCMAKE_TOOLCHAIN_FILE=$tc"
```
- -S . = source directory
- -B build = put outputs in build/
- -G Ninja = use Ninja generator
- -D… = define variables (toolchain here)
2. Normal Builds (Every time you edit a .c/.h file):
```
cmake --build build -j
```
- Ninja recompiles only what changed. Super fast.
3. Flash
- VS Code: Ctrl+Shift+B --> Flash (OpenOCD, HEX)
- CLI
```
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -c "transport select swd; adapter speed 950; init; reset halt; flash write_image erase build/CNCv1.hex; verify_image build/CNCv1.hex; reset run; exit"
```
4. Debug
- Press F5 in VS Code

## Clean and rebuild commands
- Safe workflow (Standard build):
```
cmake --build build -j
```
- Clean then build:
```
cmake --build build --clean-first -j
```
- True fresh build (delete cache + files):
```
cmake -E rm -rf build
$tc = (Resolve-Path .\cmake\toolchain-arm-none-eabi.cmake).Path
cmake -S . -B build -G Ninja "-DCMAKE_TOOLCHAIN_FILE=$tc"
cmake --build build -j
```
or
```
rm -r build
cmake -S . -B build -G Ninja "-DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-arm-none-eabi.cmake"
```

- Just clean (keep CMake files and If you only want to delete object files but keep configuration:):
```
ninja -C build clean
```
or
```
cmake --build build --target clean
```

- SUMMARY: Use clean build only after big changes (new linker script, toolchain file edits, etc.).
- This is a continuation of CNCv1 STM32 bare-metal project. Repo is structured with app/, drivers/, bsp/, etc.

