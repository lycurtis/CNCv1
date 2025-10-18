# src/config/CMakeLists.txt

add_library(cfg STATIC system_clock.c)

target_include_directories(cfg PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(cfg PUBLIC fw_opts cmsis_headers)
