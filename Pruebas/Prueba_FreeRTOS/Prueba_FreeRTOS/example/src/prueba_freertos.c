/*
 * @brief FreeRTOS examples
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2014
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
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include <stdlib.h>

/****************************************************************************
 * Prototipos de funciones
 ****************************************************************************/
float pid(float err,float err_acum,float err_prev);		//Controlador PID
void SetupHardware(void);				//Configuracion del hardware
void ConfigureADC(void);				//Configuracion del ADC
void ConfigurePhaseDetector(void);	//Configuracion del detector de cruce por cero
void ConfigureTimerFireTrigger(void);			//Configuracion del timer que dispara el trigger
void ConfigureTimerTriggerOn(void);			//Configuracion del timer que regula la duracion del trigger
static void vHandlerZeroCrossing(void *pvParameters);	//Tarea de deteccion de cruce por cero
static void vHandlerFireTrigger(void *pvParameters);	//Tarea que dispara el trigger
static void vHandlerPID(void *pvParameters);			//Tarea que aplica el PID
/************************************************************/

/*************************************************************************
 * Variables gloables
 *************************************************************************/
xSemaphoreHandle xPhaseSemaphore;	//Semaforo para la deteccion de cruce por cero
xSemaphoreHandle xFireTriggerSemaphore;	//Semaforo para la activacion del timer que dispara el trigger
xSemaphoreHandle xPIDSemaphore;		//Semaforo para la ejecucion del PID

volatile uint16_t adc_data;			//Valor leido en el ADC
ADC_CLOCK_SETUP_T ADCSetup;			//Estructura de condiguracion del ADC
#define ADC_INTERRUPT_PRIORITY	5	//Prioridad de la interrupcion del ADC

/* Como el calentamiento de las resistencias es lento, es convieniento hacer
 * los muestreos del ADC a una velocidad relativamente baja. Por lo tanto,
 * cuento varios ciclos de la senoidal hasta hacer una nueva lectura.
 */
uint8_t cycles_count=0;				//Cantidad de semiciclos contados
#define MAX_CYCLES	50				//Cantidad de semiciclos de la senoidal hasta que se actualiza el ADC

/* Pin de deteccion de fase */
#define PHASE_SCU_INT_PORT	6			//Puerto del SCU usado para la deteccion de fase
#define PHASE_SCU_INT_PIN	1				//Pin del SCU usado para la deteccion de fase
#define PHASE_GPIO_INT_PORT	3			//Puerto del GPIO usado para la deteccion de fase
#define PHASE_GPIO_INT_PIN	0			//Pin del GPIO usado para la deteccion de fase
#define PHASE_PININT_INDEX	0			//Canal de las interrupciones para el cruce por cero
#define PHASE_INTERRUPT_PRIORITY	5		//Prioridad de la interrupcion del detector de cruce por cero

/* Pin de trigger del triac */
#define TRIGGER_SCU_INT_PORT	6			//Puerto del SCU usado para el trigger del triac
#define TRIGGER_SCU_INT_PIN		5			//Pin del SCU usado para el trigger del triac
#define TRIGGER_GPIO_INT_PORT	3			//Puerto del GPIO usado para el trigger del triac
#define TRIGGER_GPIO_INT_PIN	4			//Pin del GPIO usado para el trigger del triac

#define FIRE_TRIGGER_TIMER				LPC_TIMER0		//Timer para disparar el trigger
#define FIRE_TRIGGER_CHANNEL			0				//Canal del timer que dispara el trigger
#define FIRE_TRIGGER_RST				RGU_TIMER0_RST	//Reset del timer que dispara el trigger
#define FIRE_TRIGGER_IRQ_HANDLER		TIMER0_IRQHandler	//Handler del timer que dispara el trigger
#define FIRE_TRIGGER_IRQn				TIMER0_IRQn		//Interrupcion del timer que dispara el trigger

#define TRIGGER_ON_TIMER				LPC_TIMER1		//Timer para disparar el trigger
#define TRIGGER_ON_CHANNEL				1				//Canal del timer que dispara el trigger
#define TRIGGER_ON_RST					RGU_TIMER1_RST	//Reset del timer que dispara el trigger
#define TRIGGER_ON_IRQ_HANDLER			TIMER1_IRQHandler	//Handler del timer que dispara el trigger
#define TRIGGER_ON_IRQn					TIMER1_IRQn		//Interrupcion del timer que dispara el trigger

#define TRIGGER_ON_DELAY 				50000				//Tiempo de encendido del trigger

/* Macros para el timer. Dado que el detector de cruce por cero no es inmediato, se deben
 * poner los margenes para evitar problemas. */
#define TIMER_BASE 100000//380000
#define TIMER_MAX 1910300//1800000
#define TIMER_STEP (TIMER_MAX-TIMER_BASE)/1024
#define TIMER_INTERRUPT_PRIORITY 5

/* Variables y constantes para el controlador PID */
#define KP	5	//300
#define KI	5	//150
#define KD	5	//150
#define OPAMP_GAIN	245.4	//Ganancia del circuito amplificador de la termocupla
#define MAX_TEMPERATURE 74	//Temperatura alcanzada con 220 eficaces

const float reference=50;	//Valor de referencia para el PID
float controller_output;	//Salida del controlador

/*****************************************************************************/

/*****************************************************************************
 * Otras funciones
*****************************************************************************/
/* Uso el Hook de la tarea Idle. De esta forma tomo muestras del ADC siempre que haya
 * un intervalo de tiempo disponible.
 */
void vApplicationIdleHook(void){
	/* Si se alcanzo el valor maximo de semiciclos de la senoidal,
	 * entonces se lee un nuevo valor del ADC.
	 */
	if( cycles_count >= MAX_CYCLES ){
		cycles_count=0;		//Se resetea el contador de ciclos
		Chip_ADC_SetStartMode(LPC_ADC0, ADC_START_NOW, ADC_TRIGGERMODE_RISING);
	}
}
/**
 * @name: pid
 * @description: Esta función aplica un controlador PID
 * @param err: Diferencia entre el valor actual y el de referencia
 * @param err_acum: Error acumulado (se utiliza para el control integral)
 * @param err_prev: Error del paso previo (se utiliza para el control derivativo)
 * @return: Retorna la salida del controlador
 */
float pid( float err , float err_acum , float err_prev ){
	return (KP*err);//+KI*(err_acum+err)+KD*(err-err_prev));
}
/*****************************************************************************/


/*****************************************************************************
 * Handlers de interrupciones
*****************************************************************************/
/* Handler del ADC0 */
void ADC0_IRQHandler(void){
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	Chip_ADC_ReadValue(LPC_ADC0, ADC_CH0,&adc_data);	//Se hace una nueva lectura de la temperatura

	xSemaphoreGiveFromISR(xPIDSemaphore, &xHigherPriorityTaskWoken);	//Se otorga el semaforo
	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);		//Se fuerza un cambio de contexto si es necesario
}

/* Handler de las interrupciones del GPIO */
void GPIO0_IRQHandler(void){
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(PHASE_PININT_INDEX));	//Se limpian las interrupciones

	xSemaphoreGiveFromISR(xPhaseSemaphore, &xHigherPriorityTaskWoken);	//Se otorga el semaforo
	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);		//Se fuerza un cambio de contexto si es necesario
}

/* Handler del timer */
void FIRE_TRIGGER_IRQ_HANDLER(void){
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	Chip_TIMER_Disable(FIRE_TRIGGER_TIMER);	//Se apaga el timer. El timer se debe activar solo cuando se lo requiere
	Chip_TIMER_ClearMatch(FIRE_TRIGGER_TIMER, FIRE_TRIGGER_CHANNEL);	//Se limpian las interrupciones pendientes

	xSemaphoreGiveFromISR(xFireTriggerSemaphore, &xHigherPriorityTaskWoken);	//Se otorga el semaforo del timer
	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);		//Se fuerza un cambio de contexto si es necesario
}

/* Handler del timer */
void TRIGGER_ON_IRQ_HANDLER(void){
	Chip_TIMER_Disable(TRIGGER_ON_TIMER);	//Se apaga el timer. El timer se debe activar solo cuando se lo requiere
	Chip_TIMER_ClearMatch(TRIGGER_ON_TIMER, TRIGGER_ON_CHANNEL);	//Se limpian las interrupciones pendientes
	Chip_GPIO_SetPinState(LPC_GPIO_PORT, TRIGGER_GPIO_INT_PORT, TRIGGER_GPIO_INT_PIN, (bool) false);
}
/******************************************************************************/


/*****************************************************************************
 * Funciones de configuracion
*****************************************************************************/
/* Sets up system hardware */
void SetupHardware(void){
	SystemCoreClockUpdate();
	Board_Init();

	ConfigureADC();	//Configuracion del ADC
	ConfigurePhaseDetector();	//Configuracion de las interrupciones para el detector de fase
	ConfigureTimerFireTrigger();	//Se configura el timer para la generacion del delay del trigger
	ConfigureTimerTriggerOn();
}

/* Configuracion del ADC */
void ConfigureADC( void ){
	Chip_ADC_Init(LPC_ADC0, &ADCSetup);						//Se inicializa el ADC
	Chip_ADC_EnableChannel(LPC_ADC0, ADC_CH0, ENABLE);		//Se habilita el canal 0
	NVIC_SetPriority(ADC0_IRQn, ADC_INTERRUPT_PRIORITY);	//Se setea la prioridad de la interrupcion
	NVIC_EnableIRQ(ADC0_IRQn);								//Se habilita las interrupciones
	Chip_ADC_Int_SetChannelCmd(LPC_ADC0, ADC_CH0, ENABLE);	//Se habilita las interrupciones del canal 0
}

/* Configuro las interrupciones externas generadas por el detector de fase.*/
void ConfigurePhaseDetector( void ){

	/* Se configura el pin de deteccion para el modo GPIO */
	Chip_SCU_PinMuxSet(PHASE_SCU_INT_PORT, PHASE_SCU_INT_PIN,(SCU_MODE_INBUFF_EN|SCU_MODE_INACT|SCU_MODE_FUNC0) );

	/* Se configura el pin como entrada */
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, PHASE_GPIO_INT_PORT, PHASE_GPIO_INT_PIN);

	/* Se configura el canal PININT_INDEX para las interrupciones */
	Chip_SCU_GPIOIntPinSel(PHASE_PININT_INDEX, PHASE_GPIO_INT_PORT, PHASE_GPIO_INT_PIN);

	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(PHASE_PININT_INDEX));	//Se limpian las interrupciones
	Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(PHASE_PININT_INDEX));	//Se setea el trigger por flancos
	LPC_GPIO_PIN_INT->SIENR |= PININTCH(PHASE_PININT_INDEX);					//Se setean los flancos ascendentes
	LPC_GPIO_PIN_INT->SIENF |= PININTCH(PHASE_PININT_INDEX);					//Se setean los flancos descendentes

	/* Habilitacion de las interrupciones */
	NVIC_SetPriority(PIN_INT0_IRQn, PHASE_INTERRUPT_PRIORITY);	//Se setea la prioridad de la interrupcion
	NVIC_ClearPendingIRQ(PIN_INT0_IRQn);						//Se limpian las interrupciones pendientes
	NVIC_EnableIRQ(PIN_INT0_IRQn);								//Se habilitan las interrupciones para el pin
}

/* Configuracion del timer */
void ConfigureTimerFireTrigger( void ){
	Chip_TIMER_Init(FIRE_TRIGGER_TIMER);		//Se habilita el timer
	Chip_RGU_TriggerReset(FIRE_TRIGGER_RST);	//Se resetea el periferico del timer
	while (Chip_RGU_InReset(FIRE_TRIGGER_RST));
	Chip_TIMER_Reset(FIRE_TRIGGER_TIMER);	//Se resetea el timer y se inicializa en cero

	/* Se habilitan las interrupciones por match. Cuando el timer alcanza el valor
	 * del registro, se dispara la interrupcion. */
	Chip_TIMER_MatchEnableInt(FIRE_TRIGGER_TIMER, FIRE_TRIGGER_CHANNEL);
	/* Se inicializa el valor maximo del timer */
	Chip_TIMER_SetMatch(FIRE_TRIGGER_TIMER, FIRE_TRIGGER_CHANNEL,0);
	/* Se resetea al timer automaticamente luego de alcanzar el maximo */
	Chip_TIMER_ResetOnMatchEnable(FIRE_TRIGGER_TIMER, FIRE_TRIGGER_CHANNEL);

	NVIC_SetPriority(FIRE_TRIGGER_IRQn, TIMER_INTERRUPT_PRIORITY);	//Se setea la prioridad de la interrupcion del timer

	NVIC_EnableIRQ(FIRE_TRIGGER_IRQn);				//Se habilitan las interrupciones del timer
	NVIC_ClearPendingIRQ(FIRE_TRIGGER_IRQn);		//Se limpian las interrupciones pendientes del timer

	/* Configuracion del pin de trigger del triac */
	Chip_SCU_PinMuxSet(TRIGGER_SCU_INT_PORT, TRIGGER_SCU_INT_PIN,(SCU_MODE_INBUFF_EN|SCU_MODE_INACT|SCU_MODE_FUNC0));
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, TRIGGER_GPIO_INT_PORT, TRIGGER_GPIO_INT_PIN);	//Pin de salida
	Chip_GPIO_SetPinState(LPC_GPIO_PORT, TRIGGER_GPIO_INT_PORT, TRIGGER_GPIO_INT_PIN, (bool) false);
}

/* Configuracion del timer */
void ConfigureTimerTriggerOn( void ){
	Chip_TIMER_Init(TRIGGER_ON_TIMER);		//Se habilita el timer
	Chip_RGU_TriggerReset(TRIGGER_ON_RST);	//Se resetea el periferico del timer
	while (Chip_RGU_InReset(TRIGGER_ON_RST));
	Chip_TIMER_Reset(TRIGGER_ON_TIMER);				//Se resetea el timer y se inicializa en cero

	/* Se habilitan las interrupciones por match. Cuando el timer alcanza el valor
	 * del registro, se dispara la interrupcion. */
	Chip_TIMER_MatchEnableInt(TRIGGER_ON_TIMER, TRIGGER_ON_CHANNEL);
	/* Se inicializa el valor maximo del timer */
	Chip_TIMER_SetMatch(TRIGGER_ON_TIMER, TRIGGER_ON_CHANNEL,0);
	/* Se resetea al timer automaticamente luego de alcanzar el maximo */
	Chip_TIMER_ResetOnMatchEnable(TRIGGER_ON_TIMER, TRIGGER_ON_CHANNEL);

	NVIC_SetPriority(TRIGGER_ON_IRQn, TIMER_INTERRUPT_PRIORITY);//Se setea la prioridad de la interrupcion del timer

	NVIC_EnableIRQ(TRIGGER_ON_IRQn);			//Se habilitan las interrupciones del timer
	NVIC_ClearPendingIRQ(TRIGGER_ON_IRQn);		//Se limpian las interrupciones pendientes del timer

}
/*****************************************************************************/


/*****************************************************************************
 * Definicion de las tareas
*****************************************************************************/
/* Esta tarea se ejecuta siempre que se produzca un cruce por cero.*/
static void vHandlerZeroCrossing(void *pvParameters){

	/* Se toma el semaforo para vaciarlo antes de entrar al loop infinito */
    xSemaphoreTake(xPhaseSemaphore, (portTickType) 0);

	while (1) {

		/* La tarea permanece bloqueada hasta que el semaforo se libera */
        xSemaphoreTake(xPhaseSemaphore, portMAX_DELAY);

        cycles_count++;		//Se cuenta un nuevo ciclo de la senoidal

        /* Se configura el valor maximo del timer */
        Chip_TIMER_SetMatch(FIRE_TRIGGER_TIMER, FIRE_TRIGGER_CHANNEL,controller_output);//TIMER_BASE+TIMER_STEP*adc_data);
        /* Se habilita el timer para disparar el trigger */
        Chip_TIMER_Enable(FIRE_TRIGGER_TIMER);
    }
}

/* Esta tarea se ejecuta siempre que se deba disparar el trigger, es
 * decir que el timer finalizo la cuenta.*/
static void vHandlerFireTrigger(void *pvParameters){

	/* Se toma el semaforo para vaciarlo antes de entrar al loop infinito */
    xSemaphoreTake(xFireTriggerSemaphore, (portTickType) 0);

	while (1) {

		/* La tarea permanece bloqueada hasta que el semaforo se libera */
        xSemaphoreTake(xFireTriggerSemaphore, portMAX_DELAY);

		/* Se dispara el trigger */
		Chip_GPIO_SetPinState(LPC_GPIO_PORT, TRIGGER_GPIO_INT_PORT, TRIGGER_GPIO_INT_PIN, (bool) true);

		/* Se activa el timer que controla la duracion del trigger */
		Chip_TIMER_SetMatch(TRIGGER_ON_TIMER, TRIGGER_ON_CHANNEL,TRIGGER_ON_DELAY);
		Chip_TIMER_Enable(TRIGGER_ON_TIMER);

    }
}

/* Esta tarea aplica el controlador PID a las lecturas del ADC .*/
static void vHandlerPID(void *pvParameters){

	static float err=0;			//Error del PID
	static float err_acum=0;	//Error acumulado para el PID
	float err_prev=0;			//Error del paso previo del PID
	float temp_termocupla;		//Temperatura medida con la termocupla

	/* Se toma el semaforo para vaciarlo antes de entrar al loop infinito */
    xSemaphoreTake(xPIDSemaphore, (portTickType) 0);

	while (1) {

		/* La tarea permanece bloqueada hasta que el semaforo se libera */
        xSemaphoreTake(xPIDSemaphore, portMAX_DELAY);

		temp_termocupla = ((float)adc_data)*3.3/1024;		//Lo mapeo de 0-3.3v
		temp_termocupla /= OPAMP_GAIN;						//Lo divido por la ganancia del operacional
		temp_termocupla=23376.525*temp_termocupla-1.574;	//Mapeo la temperatura

		/*if( temp_termocupla >= reference )
			controller_output = TIMER_MAX;
		else
			controller_output = TIMER_BASE;*/
        /* Aplicacion del PID */
		err_prev=err;					//Error del paso previo
		err=reference-temp_termocupla;	//Error de la salida
		err_acum=err_acum+err;			//Integral del error (valor acumulado)
		controller_output=TIMER_STEP*pid(err,err_acum,err_prev);	//Se aplica el controlador

		if( controller_output <= 0 )
			controller_output = TIMER_MAX;
		else if( controller_output > TIMER_MAX )
			controller_output = TIMER_MAX;
		else if( controller_output < TIMER_BASE )
			controller_output = TIMER_BASE;

		//controller_output= TIMER_BASE+TIMER_STEP*controller_output;//TIMER_MAX*(1-controller_output/MAX_TEMPERATURE);	//Se transforma la salida un valor de delay

    }
}
/*****************************************************************************/


/*****************************************************************************
 * Funcion main
 ****************************************************************************/
int main(void){
	SetupHardware();	//Se inicializa el hardware

	vSemaphoreCreateBinary(xPhaseSemaphore);	//Se crea el semaforo para la deteccion de cruces por cero
	vSemaphoreCreateBinary(xFireTriggerSemaphore);	//Se crea el semaforo para la activacion del timer
	vSemaphoreCreateBinary(xPIDSemaphore);		//Se crea el semaforo para el PID

	/* Se verifica que el semaforo haya sido creado correctamente */
	if( (xPhaseSemaphore!=(xSemaphoreHandle)NULL)&&(xFireTriggerSemaphore!=(xSemaphoreHandle)NULL)&&(xPIDSemaphore!=(xSemaphoreHandle)NULL) ){

		/* Se crea la tarea que se encarga de detectar los cruces por cero */
		xTaskCreate(vHandlerZeroCrossing, (char *) "ZeroCrossing", configMINIMAL_STACK_SIZE,
							(void *) 0, (tskIDLE_PRIORITY + 1UL), (xTaskHandle *) NULL);
		/* Se crea la tarea que se encarga de generar los disparos del trigger */
		xTaskCreate(vHandlerFireTrigger, (char *) "FireTrigger", configMINIMAL_STACK_SIZE,
							(void *) 0, (tskIDLE_PRIORITY + 2UL), (xTaskHandle *) NULL);
		/* Se crea la tarea que se encarga de aplicar el PID */
		xTaskCreate(vHandlerPID, (char *) "PID", configMINIMAL_STACK_SIZE,
							(void *) 0, (tskIDLE_PRIORITY + 2UL), (xTaskHandle *) NULL);


		vTaskStartScheduler(); /* Se comienzan a ejecutar las tareas. */
	}

	while (1);	//No se deberia llegar nunca a este punto.
return ((int) NULL);
}

