# src/bsp/CMakeLists.txt

add_library(bsp STATIC
  bsp_gpio.c
  bsp_usart2_debug.c
)

target_include_directories(bsp PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(bsp PUBLIC fw_opts cmsis_headers)

