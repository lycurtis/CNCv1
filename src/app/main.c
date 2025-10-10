#include "main.h"

int main(void){
    system_clock_init(); // 180 MHz PLL setup

    stepgen_init();
    stepgen_enable(true);
    stepgen_dir(true);
    stepgen_set_hz(3500); // 1 kHz step rate
    stepgen_start();

    while(1){
        
    }
}
