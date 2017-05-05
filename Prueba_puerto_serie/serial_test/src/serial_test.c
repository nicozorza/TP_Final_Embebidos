/*
===============================================================================
 Name        : serial_test.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#include "board.h"

#define TICKRATE_HZ (1000)

static bool led_flag=false;

/**
 * @brief	Handle interrupt from SysTick timer
 * @return	Nothing
 */
static uint32_t tick_ct = 0;
void SysTick_Handler(void)
{

	tick_ct += 1;
	if ((tick_ct % 100) == 0) {
		led_flag=true;
	}
}


int main(void) {

	SystemCoreClockUpdate();
    Board_Init();
    SysTick_Config(SystemCoreClock / TICKRATE_HZ);

    while(1) {
        __WFI();
        if( led_flag == true ){
        	Board_LED_Toggle(0);
        	led_flag=false;
        }
    }
    return 0 ;
}
