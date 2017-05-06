/*
===============================================================================

 Name        : adc_test.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#include "board.h"

#define TICKRATE_HZ1 1000
/**
 * @brief	Handle interrupt from SysTick timer
 * @return	Nothing
 */
static uint32_t tick_ct = 0;
static bool led_flag=false;
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
    Board_LED_Set(0, true);

    /* Enable and setup SysTick Timer at a periodic rate */
    SysTick_Config(SystemCoreClock / TICKRATE_HZ1);

    while(1) {
        __WFI();
        if( led_flag == true ){
			Board_LED_Toggle(0);
			led_flag=false;
		}

    }
return 0 ;
}
