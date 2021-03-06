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
#define BOARD_NXP_LPCXPRESSO_4337

#define TICKRATE_HZ (1000)	/* 1000 ticks per second */
#define SCT_PWM_RATE   10000        /* PWM frequency 10 KHz */
#define DUTY_CYCLE 50/100	//Duty Cycle configurado


volatile bool adc_flag=false;
volatile bool systick_flag=false;
uint16_t adc_data=0;

/* Variables para el PID */
float pwm=0;
const float reference=75;	//Valor de referencia para el PID (porcentaje)
float err=0;	//Error del PID
float err_acum=0;	//Error acumulado para el PID
float err_prev=0;	//Error del paso previo del PID

#define KP 0.1
#define KI 0.1
#define KD 0.1
/*************************/

/*******************************************************************/
void ADC0_IRQHandler(void){
	Chip_ADC_ReadValue(LPC_ADC0, ADC_CH0,&adc_data);	//Se lee el valor del ADC para limpiar interrupciones
	adc_flag=true;
}

ADC_CLOCK_SETUP_T ADCSetup;

void init_adc(void){
	Chip_ADC_Init(LPC_ADC0, &ADCSetup);
	Chip_ADC_EnableChannel(LPC_ADC0, ADC_CH0, ENABLE);
	NVIC_EnableIRQ(ADC0_IRQn);
	Chip_ADC_Int_SetChannelCmd(LPC_ADC0, ADC_CH0, ENABLE);
	//Chip_ADC_SetBurstCmd(LPC_ADC0, ENABLE);
}
/*******************************************************************/
void init_pwm(void){
	/* Initialize the SCT as PWM and set frequency */
	Chip_SCTPWM_Init(LPC_SCT);
	Chip_SCTPWM_SetRate(LPC_SCT, SCT_PWM_RATE);

	/* Setup Board specific output pin */
	Chip_SCU_PinMuxSet(0x4, 4, (SCU_MODE_INACT | SCU_MODE_FUNC1));	//CTOUT2=P4_4

	/* Use SCT0_OUT1 pin */
	Chip_SCTPWM_SetOutPin(LPC_SCT, 1, 2);

	/* Set duty cycle */
	Chip_SCTPWM_SetDutyCycle(LPC_SCT, 1, Chip_SCTPWM_GetTicksPerCycle(LPC_SCT)*DUTY_CYCLE );
	Chip_SCTPWM_Start(LPC_SCT);
}
/*******************************************************************/
/**
 * @name: pid
 * @description: Esta función aplica un controlador PID
 * @param err: Diferencia entre el valor actual y el de referencia
 * @param err_acum: Error acumulado (se utiliza para el control integral)
 * @param err_prev: Error del paso previo (se utiliza para el control derivativo)
 * @return: Retorna la salida del controlador
 */
float pid( float err , float err_acum , float err_prev ){
	return -(KP*err+KI*(err_acum+err)+KD*(err-err_prev));
}

/*******************************************************************/
void SysTick_Handler(void){
	systick_flag=true;
}
/*******************************************************************/

int main(void)
{

	SystemCoreClockUpdate();
	Board_Init();

	/* Enable and setup SysTick Timer at a periodic rate */
	//SysTick_Config(SystemCoreClock / TICKRATE_HZ);


	/* Inicializacion del PWM */
	init_pwm();
	/* Inicializacion del ADC */
	init_adc();
	while (1) {
		Chip_ADC_SetStartMode(LPC_ADC0, ADC_START_NOW, ADC_TRIGGERMODE_RISING);	//Se inicia un muestreo del ADC
		__WFI();
		if( adc_flag==true){

			/* Aplicacion del PID */
			err_prev=err;					//Error del paso previo
			err=pwm*100/1024-reference;//(float)adc_data*100/1024;	//Error de la salida
			err_acum=err_acum+err;			//Integral del error (valor acumulado)
			pwm=pid(err,err_acum,err_prev);	//Se aplica el controlador
			/**********************/

			Chip_SCTPWM_SetDutyCycle(LPC_SCT, 1, Chip_SCTPWM_GetTicksPerCycle(LPC_SCT)*pwm/1024 );
			DEBUGOUT("%f\n\r",pwm*100/1024);

			adc_flag=false;
		}
	}
}

