# src/drivers/stepgen/CMakeLists.txt

add_library(stepgen STATIC
  stepgen_pwm_tim3.c
)
target_include_directories(stepgen PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(stepgen
  PUBLIC  fw_opts cmsis_headers  # headers/flags propagate to users of stepgen
  PUBLIC  bsp                     # stepgen uses bsp_gpio.h / bsp_pins.h
)
