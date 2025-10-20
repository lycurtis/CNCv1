# src/config/clock/CMakeLists.txt

add_library(clock STATIC
  system_clock.c
)

# Publish this folder so #include "system_clock.h" works
target_include_directories(clock PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}
)

# Uses CMSIS/device headers
target_link_libraries(clock PUBLIC
  fw_opts
  cmsis_headers
)
