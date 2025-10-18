# CNCv1 – STM32 Bare-Metal + CMake + VS Code

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
   - Arm Assembly
   - C/C++
   - C/C++ Extension Pack
   - CMake
   - CMake Tools
   - Cortex-Debug
   - LinkerScript
   - MemoryView
   - Peripheral Viewer
   - RTOS Views
   
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

## Typical Workflow
1) Configure (creates build/Debug/)
```
cmake --preset debug
```
- This reads CMakePresets.json, sets the toolchain, and generates Ninja files into build/Debug

2) Build
```
cmake --build --preset debug
```
- Produces CNCv1.elf/.bin/.hex in build/Debug/

3) Clean vs. Reconfigure
- Clean compiled objects (keep cache):
```
cmake --build --preset debug --target clean
```
- Full reset (wipe cache and files):
```
cmake -E remove_directory build\Debug
// or
Remove-Item -Path .\build\Debug -Recurse -Force
cmake --preset debug
cmake --build --preset debug
```

4) Useful extras
- Rebuild a specific target:
```
cmake --build --preset debug --target CNCv1
```
- See available targets:
```
cmake --build --preset debug --target help
```

## VS Code (CMake Tools)
- Configure: Command Palette (ctrl+shift+p) -> "CMake: Delete Cache and Reconfigure" (or "CMake: Configure" if it's the first time)
- Build: “CMake: Build”
- Clean: “CMake: Clean”
- Ensure the active kit/preset is your ARM preset (“ARM Debug”)

## Statement
This is a continuation of CNCv1 STM32F446RE bare-metal project. Repo is structured with app/, drivers/, bsp/, etc.

