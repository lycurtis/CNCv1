#include "delay.h"

void delay(volatile uint32_t time){
    while (time--) { __NOP(); } // NOP (No OPeration) = “waste 1 CPU cycle, do nothing.”
    /*
    Without __NOP(), the compiler sees while(time--); as “does nothing useful” → deletes or shrinks it.
    With __NOP(), the compiler is forced to keep the loop, because the nop is a real instruction with side effects (time).
    */
}
