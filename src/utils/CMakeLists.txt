# src/utils/CMakeLists.txt

add_library(utils STATIC
  delay.c
)

target_include_directories(utils PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}
)

# Needs CMSIS because delay.c includes <stm32f4xx.h> (for __NOP)
target_link_libraries(utils PUBLIC
  fw_opts
  cmsis_headers
)
