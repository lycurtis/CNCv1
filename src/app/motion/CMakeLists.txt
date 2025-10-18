# src/app/motion/CMakeLists.txt

add_library(motion STATIC
  motion_units.c
)

# motion’s own headers
target_include_directories(motion PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}
)

# CRUCIAL: motion uses stepgen headers (motion_units.h includes stepgen_pwm_tim3.h)
# Link PUBLIC so consumers of motion also inherit this requirement.
target_link_libraries(motion PUBLIC
  stepgen
  fw_opts
  cmsis_headers
)
