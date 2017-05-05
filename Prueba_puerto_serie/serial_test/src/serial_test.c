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

void init_uart(LPC_USART_T *pUART, int baudrate ){
	Board_UART_Init(pUART);
	Chip_UART_Init(pUART);
	Chip_UART_SetBaudFDR(pUART, baudrate);
	Chip_UART_ConfigData(pUART, UART_LCR_WLEN8 | UART_LCR_SBS_1BIT | UART_LCR_PARITY_DIS);

	/* Enable UART Transmit */
	Chip_UART_TXEnable(pUART);
}
int main(void) {

	SystemCoreClockUpdate();
    Board_Init();
    SysTick_Config(SystemCoreClock / TICKRATE_HZ);

    init_uart(LPC_USART2,9600);

    while(1) {
        __WFI();
        if( led_flag == true ){
        	Board_LED_Toggle(0);
        	led_flag=false;
        	Board_UARTPutSTR("hola\n\r");
        }
    }
    return 0 ;
}
