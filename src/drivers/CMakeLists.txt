# src/drivers/CMakeLists.txt
add_library(drivers_obj OBJECT 
 stepgen_pwm_tim3.c
)

# (Optional: add headers so they show up in IDEs)
# target_sources(drivers_obj PRIVATE
#   stepgen_pwm_tim3.h
# )
