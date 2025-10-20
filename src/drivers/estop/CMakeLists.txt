#src/drivers/estop/CMakeLists.txt

add_library(estop  STATIC
    estop.c
)

target_include_directories(estop PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(estop PUBLIC
    fw_opts
    cmsis_headers
    bsp
)
