#include "board.h"

#define TICKRATE_HZ (1000)	/* 1000 ticks per second */

volatile bool systick_flag=false;
volatile bool adc_flag0=false;
volatile bool adc_flag1=false;
volatile uint16_t adc_data0=0;
volatile uint16_t adc_data1=0;

#define OPAMP_GAIN 245.4		//Ganancia del circuito amplificador de la termocupla
#define PT100_PULLUP 100	//Resistencia conectada al PT100 para el divisor resistivo

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
	adc_flag0=true;
	Chip_ADC_ReadValue(LPC_ADC0, ADC_CH0,&adc_data0);
}

void ADC1_IRQHandler(void){
	adc_flag1=true;
	Chip_ADC_ReadValue(LPC_ADC1, ADC_CH1,&adc_data1);
}

ADC_CLOCK_SETUP_T ADCSetup0;
ADC_CLOCK_SETUP_T ADCSetup1;

/**
 * @brief	main routine for blinky example
 * @return	Function should not exit.
 */
int main(void)
{
	float temp_termocupla, temp_pt100;

	SystemCoreClockUpdate();
	Board_Init();

	/* Enable and setup SysTick Timer at a periodic rate */
	SysTick_Config(SystemCoreClock / TICKRATE_HZ);

	/* Inicializacion del ADC0 */
	Chip_ADC_Init(LPC_ADC0, &ADCSetup0);
	Chip_ADC_EnableChannel(LPC_ADC0, ADC_CH0, ENABLE);
	NVIC_EnableIRQ(ADC0_IRQn);
	Chip_ADC_Int_SetChannelCmd(LPC_ADC0, ADC_CH0, ENABLE);
	Chip_ADC_SetStartMode(LPC_ADC0, ADC_START_NOW, ADC_TRIGGERMODE_RISING);

	/* Inicializacion del ADC1 */
	Chip_ADC_Init(LPC_ADC1, &ADCSetup1);
	Chip_ADC_EnableChannel(LPC_ADC1, ADC_CH1, ENABLE);
	NVIC_EnableIRQ(ADC1_IRQn);
	Chip_ADC_Int_SetChannelCmd(LPC_ADC1, ADC_CH1, ENABLE);
	Chip_ADC_SetStartMode(LPC_ADC1, ADC_START_NOW, ADC_TRIGGERMODE_RISING);

	while (1) {
		__WFI();
		if( adc_flag0==true){									//Medicion de la termocupla
			temp_termocupla = ((float)adc_data0)*3.3/1023;		//Lo mapeo de 0-3.3v
			temp_termocupla /= OPAMP_GAIN;						//Lo divido por la ganancia del operacional
			temp_termocupla=23376.525*temp_termocupla-1.574;	//Mapeo la temperatura

			adc_flag0=false;
			Chip_ADC_SetStartMode(LPC_ADC0, ADC_START_NOW, ADC_TRIGGERMODE_RISING);
		}
		if( adc_flag1==true){										//Medicion del PT100. Se hace a traves de un divisor resistivo
			temp_pt100 = ((float)adc_data1)*3.3/1023;				//Lo mapeo de 0-3.3v
			if( 3.3!=temp_pt100)
				temp_pt100 = temp_pt100*PT100_PULLUP/(3.3-temp_pt100);	//Se despeja la resistencia del PT100 del divisor resistivo
			else
				temp_pt100 = temp_pt100*PT100_PULLUP/(0.0001);			//Si son iguales divido por un numero pequeño. Es re cabeza
			temp_pt100 = 2.857*temp_pt100-239.178;					//Se obtiene la temperatura

			adc_flag1=false;
			Chip_ADC_SetStartMode(LPC_ADC1, ADC_START_NOW, ADC_TRIGGERMODE_RISING);
		}

		if( systick_flag == true ){
			systick_flag=false;

			DEBUGOUT("Termocupla: %f°C\n\r",temp_termocupla);
			DEBUGOUT("PT100: %f°C\n\r",temp_pt100);
		}
	}
}