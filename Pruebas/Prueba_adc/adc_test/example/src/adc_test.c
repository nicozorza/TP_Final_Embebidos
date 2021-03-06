/*
 * @brief Blinky example using sysTick
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2013
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#include "board.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

#define TICKRATE_HZ (1000)	/* 1000 ticks per second */


volatile bool adc_flag=false;
volatile bool systick_flag=false;
uint16_t adc_data=0;

/**
 * @brief	Handle interrupt from SysTick timer
 * @return	Nothing
 */
static uint32_t tick_ct = 0;
void SysTick_Handler(void)
{
	tick_ct += 1;
	if ((tick_ct % 100) == 0) {
		systick_flag=true;
	}
}

void ADC0_IRQHandler(void){
	adc_flag=true;
	Chip_ADC_ReadValue(LPC_ADC0, ADC_CH0,&adc_data);
}
ADC_CLOCK_SETUP_T ADCSetup;

/**
 * @brief	main routine for blinky example
 * @return	Function should not exit.
 */
int main(void)
{

	SystemCoreClockUpdate();
	Board_Init();

	/* Enable and setup SysTick Timer at a periodic rate */
	SysTick_Config(SystemCoreClock / TICKRATE_HZ);

	/* Inicializacion del ADC */
	Chip_ADC_Init(LPC_ADC0, &ADCSetup);
	Chip_ADC_EnableChannel(LPC_ADC0, ADC_CH0, ENABLE);
	NVIC_EnableIRQ(ADC0_IRQn);
	Chip_ADC_Int_SetChannelCmd(LPC_ADC0, ADC_CH0, ENABLE);
	//Chip_ADC_SetBurstCmd(LPC_ADC0, ENABLE);
	Chip_ADC_SetStartMode(LPC_ADC0, ADC_START_NOW, ADC_TRIGGERMODE_RISING);

	while (1) {
		__WFI();
		if( adc_flag==true){
			DEBUGOUT("%i\n\r",adc_data);
			adc_flag=false;
			Chip_ADC_SetStartMode(LPC_ADC0, ADC_START_NOW, ADC_TRIGGERMODE_RISING);
		}
		if( systick_flag==true){
			Board_LED_Toggle(3);
			systick_flag=false;
		}
	}
}
