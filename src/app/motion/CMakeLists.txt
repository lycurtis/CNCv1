# src/app/motion/CMakeLists.txt

add_library(motion STATIC
  motion_units.c
)
target_include_directories(motion PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(motion PUBLIC fw_opts)   # add cmsis_headers if it uses CMSIS
