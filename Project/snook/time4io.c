#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */

int getbtns (){
    int n = PORTD;
    n = n & 0xe0; //bit 5-7 mask
    n = n >> 4;   //shift bits 4 steps to the right for buttons to give the value of their button number

    int m = PORTF;
    m = m & 0x2;  //bit 2 mask
    m = m >> 1;   //shift bits one step so button one has value of 1

    return n | m;
}


