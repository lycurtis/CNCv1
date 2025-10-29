# src/config/axis/CMakeLists.txt

# Header-only "interface" library since axis.c doesn't exist
add_library(axis INTERFACE)

# Publish this folder so #include "axis.h" works
target_include_directories(axis INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}
)

