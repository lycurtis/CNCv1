# src/bsp/CMakeLists.txt
add_library(bsp_obj OBJECT
  bsp_gpio.c
  bsp_usart2_debug.c
)

# (Optional: add headers so they show up in IDEs)
# target_sources(bsp_obj PRIVATE
#   bsp_gpio.h
#   bsp_usart2_debug.h
#   bsp_pins.h
# )
