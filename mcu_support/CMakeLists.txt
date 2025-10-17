# mcu_support/CMakeLists.txt
add_library(cmsis_headers INTERFACE)
target_include_directories(cmsis_headers INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/Drivers/CMSIS/Include
  ${CMAKE_CURRENT_LIST_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Include
)

add_library(stm32_startup OBJECT
  st/system_stm32f4xx.c
  st/Startup/startup_stm32f446xx.s
)
target_link_libraries(stm32_startup PRIVATE cmsis_headers fw_opts)
