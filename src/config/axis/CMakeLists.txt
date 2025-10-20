# src/config/axis/CMakeLists.txt

add_library(axis STATIC
  axis.c
)

# Publish this folder so #include "system_clock.h" works
target_include_directories(axis PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}
)

# Uses CMSIS/device headers
target_link_libraries(axis PUBLIC
  fw_opts
  cmsis_headers
)
