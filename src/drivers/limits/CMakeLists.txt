# src/drivers/limits/CMakeLists.txt

add_library(limits STATIC
  limits.c
)

# so #include "limits.h" works
target_include_directories(limits PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}
)

# limits may depend on BSP + CMSIS; export those too
target_link_libraries(limits PUBLIC
  fw_opts
  cmsis_headers
  bsp
  axis
)