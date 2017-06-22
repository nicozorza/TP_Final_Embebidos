/*
===============================================================================
 Name        : com.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#define BOARD_EDU_CIAA_NXP

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif


#define ADC_TEMPERATURE			LPC_ADC0
#define ADC_CHANNEL 			ADC_CH0
#define ADC_IRQN				ADC0_IRQn
#define ADC_IRQN_HANDLER		ADC0_IRQHandler
#define ADC_INTERRUPT_PRIORITY	5
ADC_CLOCK_SETUP_T ADCSetup;			//Estructura de condiguracion del ADC
#define MAX_ADC_SAMPLES			10	//Cantidad de muestras del ADC que se promedian
#define ADC_SAMPLES_DELAY		(1/(portTICK_RATE_MS))	//Delay entre actualizaciones de la temperatura



/* Pin que detecta el apagado automatico */
#define STOP_IN_SCU_INT_PORT	6
#define STOP_IN_SCU_INT_PIN		10
#define STOP_IN_GPIO_INT_PORT	3
#define STOP_IN_GPIO_INT_PIN	6
#define STOP_IN_PININT_INDEX	0
#define STOP_IN_INTERRUPT_PRIORITY	5
#define STOP_IN_IRQN_HANDLER	GPIO0_IRQHandler
#define STOP_IN_IRQN			PIN_INT0_IRQn


/* Pin que enciende el horno */
#define START_OUT_SCU_INT_PORT		6
#define START_OUT_SCU_INT_PIN		11
#define START_OUT_GPIO_INT_PORT		3
#define START_OUT_GPIO_INT_PIN		7

/* Pin que apaga el horno */
#define STOP_OUT_SCU_INT_PORT		6
#define STOP_OUT_SCU_INT_PIN		12
#define STOP_OUT_GPIO_INT_PORT		2
#define STOP_OUT_GPIO_INT_PIN		8

#define UART LPC_USART3
#include <cr_section_macros.h>

#if defined (__MULTICORE_MASTER_SLAVE_M0APP) | defined (__MULTICORE_MASTER_SLAVE_M0SUB)
#include "cr_start_m0.h"
#endif

volatile uint16_t adc_data;
volatile bool stop_in=false;	//Variable que indica si se recibio la indicacion de stop por la finalizacion del ciclo
int flag_block_buttom=0;
char recieved_char=0;
unsigned long int i;

void ConfigureADC(void);
void Uart_Init(LPC_USART_T *pUART);
void Send_Char(char ch,LPC_USART_T *pUART);
int Recieve_Char(void);
void Start_Oven(void);
void Stop_Oven(void);
void Send_Data(void);
float get_temperature(uint16_t);
void ConfigureStopInDetector( void );

// TODO: insert other include files here

// TODO: insert other definitions and declarations here

int main(void) {

	SystemCoreClockUpdate();
	Board_Init();
	Uart_Init(UART);
	Board_LED_Toggle(5);
	ConfigureStopInDetector();
	ConfigureADC();
	while(1)
	{
		recieved_char=Recieve_Char();
		if(recieved_char=='A'&&flag_block_buttom==0)
		{
			Start_Oven();
		}
		if(recieved_char=='B'&&flag_block_buttom==1)
		{
			Stop_Oven();
		}
		if(flag_block_buttom==1)
		{
			Send_Data();
		}
		if( stop_in==true ){	//Si se recibio la indicacion de que se termino el ciclo, debe apagarse el horno
			Stop_Oven();
			stop_in=false;
		}
	}
    return 0 ;
}

void Uart_Init(LPC_USART_T *pUART)
{
	Chip_UART_Init(UART);
	Chip_UART_SetBaud(UART, 38400);  /* Set Baud rate */
	Chip_UART_SetupFIFOS(UART, UART_FCR_FIFO_EN | UART_FCR_TRG_LEV0); /* Modify FCR (FIFO Control Register)*/
	Chip_UART_TXEnable(UART); /* Enable UART Transmission */


	Chip_SCU_PinMux(2, 3, MD_PDN, FUNC2);              /* P2_3,FUNC2: UART3_TXD */
	Chip_SCU_PinMux(2, 4, MD_PLN|MD_EZI|MD_ZI, FUNC2); /* P2_4,FUNC2: UART3_RXD */

	//PIN ENCENDIDO A ZORZA
	Chip_SCU_PinMuxSet(START_OUT_SCU_INT_PORT, START_OUT_SCU_INT_PIN, (SCU_MODE_INBUFF_EN | SCU_MODE_PULLUP | SCU_MODE_FUNC0));
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, START_OUT_GPIO_INT_PORT, START_OUT_GPIO_INT_PIN);
	Chip_GPIO_SetPinState(LPC_GPIO_PORT, START_OUT_GPIO_INT_PORT, START_OUT_GPIO_INT_PIN, false);

	//PIN APAGADO A ZORZA
	Chip_SCU_PinMuxSet(STOP_OUT_SCU_INT_PORT, STOP_OUT_SCU_INT_PIN, (SCU_MODE_INBUFF_EN | SCU_MODE_PULLUP | SCU_MODE_FUNC0));
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, STOP_OUT_GPIO_INT_PORT, STOP_OUT_GPIO_INT_PIN);
	Chip_GPIO_SetPinState(LPC_GPIO_PORT, STOP_OUT_GPIO_INT_PORT, STOP_OUT_GPIO_INT_PIN, false);

}

void Send_Char(char ch,LPC_USART_T *pUART)
{

	while ((Chip_UART_ReadLineStatus(pUART) & UART_LSR_THRE) == 0) {}
	Chip_UART_SendByte(pUART, (uint8_t) ch);
}

int Recieve_Char(void)
{
if (Chip_UART_ReadLineStatus(UART) & UART_LSR_RDR) {
	return (int) Chip_UART_ReadByte(UART);
}
return EOF;
}

void Start_Oven(void)
{
	Board_LED_Toggle(4);
	Board_LED_Toggle(5);
	Chip_GPIO_SetPinState(LPC_GPIO_PORT, START_OUT_GPIO_INT_PORT, START_OUT_GPIO_INT_PIN,true); //PRENDO PIN ENCENDIDO ZORZA
	Chip_GPIO_SetPinState(LPC_GPIO_PORT, START_OUT_GPIO_INT_PORT, START_OUT_GPIO_INT_PIN,false); //APAGO PIN ENCENDIDO ZORZA

	Chip_GPIO_SetPinState(LPC_GPIO_PORT, STOP_OUT_GPIO_INT_PORT, STOP_OUT_GPIO_INT_PIN, false); //APAGO PIN APAGADO ZORZA

	recieved_char=0;
	flag_block_buttom=1;
}
void Stop_Oven(void)
{
	Board_LED_Toggle(4);
	Board_LED_Toggle(5);

	Chip_GPIO_SetPinState(LPC_GPIO_PORT, START_OUT_GPIO_INT_PORT, START_OUT_GPIO_INT_PIN,false); // APAGO PIN ENCENDIDO A ZORZA

	if( stop_in!=true ){
		Chip_GPIO_SetPinState(LPC_GPIO_PORT, STOP_OUT_GPIO_INT_PORT, STOP_OUT_GPIO_INT_PIN, true); // ENCIENDO PIN APAGADO A ZORZA
		Chip_GPIO_SetPinState(LPC_GPIO_PORT, STOP_OUT_GPIO_INT_PORT, STOP_OUT_GPIO_INT_PIN, false); // APAGO PIN APAGADO A ZORZA
	}
	recieved_char=0;
	flag_block_buttom=0;
	Send_Char(1,UART);
	Send_Char(2,UART);
}

void Send_Data(void)
{
	Chip_ADC_SetStartMode(ADC_TEMPERATURE, ADC_START_NOW, ADC_TRIGGERMODE_RISING);
	for(i=0;i<13166666;i++);
	Send_Char((int)get_temperature(adc_data),UART);
	Send_Char(2,UART);
}


float get_temperature( uint16_t adc_value ){
	float temp=0;

	temp = ((float)adc_value)*3.3/1024;		//Lo mapeo de 0-3.3v
	temp /= 160;						//Lo divido por la ganancia del operacional
	temp = 23701.6*temp-8.75; //23376.525*temp-1.574;				//Mapeo la temperatura
return temp;
}

void ConfigureADC( void ){
	Chip_ADC_Init(ADC_TEMPERATURE, &ADCSetup);						//Se inicializa el ADC
	Chip_ADC_EnableChannel(ADC_TEMPERATURE, ADC_CHANNEL, ENABLE);		//Se habilita el canal 0
	NVIC_SetPriority(ADC_IRQN, ADC_INTERRUPT_PRIORITY);	//Se setea la prioridad de la interrupcion
	Chip_ADC_Int_SetChannelCmd(ADC_TEMPERATURE, ADC_CHANNEL, ENABLE);	//Se habilita las interrupciones del canal 0
	NVIC_EnableIRQ(ADC_IRQN);								//Se habilita las interrupciones
}


void ADC0_IRQHandler(void)
{
		Chip_ADC_ReadValue(ADC_TEMPERATURE, ADC_CHANNEL,&adc_data);	//Se hace una nueva lectura de la temperatura
}





void ConfigureStopInDetector( void ){

	/* Se configura el pin de deteccion para el modo GPIO */
	Chip_SCU_PinMuxSet(STOP_IN_SCU_INT_PORT, STOP_IN_SCU_INT_PIN,(SCU_MODE_INBUFF_EN|SCU_MODE_INACT|SCU_MODE_FUNC0) );

	/* Se configura el pin como entrada */
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, STOP_IN_GPIO_INT_PORT, STOP_IN_GPIO_INT_PIN);

	/* Se configura el canal PININT_INDEX para las interrupciones */
	Chip_SCU_GPIOIntPinSel(STOP_IN_PININT_INDEX, STOP_IN_GPIO_INT_PORT, STOP_IN_GPIO_INT_PIN);

	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(STOP_IN_PININT_INDEX));	//Se limpian las interrupciones
	Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(STOP_IN_PININT_INDEX));	//Se setea el trigger por flancos
	LPC_GPIO_PIN_INT->SIENR |= PININTCH(STOP_IN_PININT_INDEX);					//Se setean los flancos ascendentes

	/* Habilitacion de las interrupciones */
	NVIC_SetPriority(STOP_IN_IRQN, STOP_IN_INTERRUPT_PRIORITY);	//Se setea la prioridad de la interrupcion
	NVIC_ClearPendingIRQ(STOP_IN_IRQN);						//Se limpian las interrupciones pendientes
	NVIC_EnableIRQ(STOP_IN_IRQN);								//Se habilitan las interrupciones para el pin
}


void STOP_IN_IRQN_HANDLER(void){
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(STOP_IN_PININT_INDEX));	//Se limpian las interrupciones
	stop_in=true;	//Se recibio la indicacion de que se termino de recorrer la curva de temperatura
}
