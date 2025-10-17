# src/CMakeLists.txt

add_library(project_includes INTERFACE)
target_include_directories(project_includes INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/app
  ${CMAKE_CURRENT_LIST_DIR}/bsp
  ${CMAKE_CURRENT_LIST_DIR}/config
  ${CMAKE_CURRENT_LIST_DIR}/drivers
  ${CMAKE_CURRENT_LIST_DIR}/utils
)

add_subdirectory(app)
add_subdirectory(bsp)
add_subdirectory(config)
add_subdirectory(drivers)
add_subdirectory(utils)

foreach(t IN ITEMS app_obj bsp_obj config_obj drivers_obj utils_obj)
  target_link_libraries(${t} PRIVATE project_includes fw_opts cmsis_headers)
endforeach()
