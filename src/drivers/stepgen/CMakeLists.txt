# src/drivers/stepgen/CMakeLists.txt

add_library(stepgen STATIC
  stepgen_pwm_tim3.c
)

# so #include "stepgen_pwm_tim3.h" works
target_include_directories(stepgen PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}
)

# stepgen may depend on BSP + CMSIS; export those too
target_link_libraries(stepgen PUBLIC
  fw_opts
  cmsis_headers
  bsp
  limits
)