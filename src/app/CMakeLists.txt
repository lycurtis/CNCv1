# src/app/CMakeLists.txt

# Submodule first (exports motion headers PUBLIC)
add_subdirectory(motion)

# App-level code (e.g., app_init.c)
add_library(app_core STATIC
  app_init.c
)

# App headers are in this folder
target_include_directories(app_core PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}
)

# app_init.c includes motion_units.h and stepgen_pwm_tim3.h
# Link PUBLIC so dependents (the executable) get their include dirs too
target_link_libraries(app_core PUBLIC
  motion
  stepgen
  fw_opts
  cfg
)
