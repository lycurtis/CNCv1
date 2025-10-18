# src/CMakeLists.txt

add_subdirectory(bsp)
add_subdirectory(config)
add_subdirectory(utils)
add_subdirectory(drivers/stepgen)
add_subdirectory(app)              # brings in "motion"
