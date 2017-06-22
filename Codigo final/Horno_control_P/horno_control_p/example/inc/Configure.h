#ifndef _CONFIGURE_H_
#define _CONFIGURE_H_

#include "board.h"

/* Configuracion del ADC */
#define ADC_TEMPERATURE			LPC_ADC0
#define ADC_CHANNEL 			ADC_CH0
#define ADC_IRQN				ADC0_IRQn
#define ADC_IRQN_HANDLER		ADC0_IRQHandler
#define ADC_INTERRUPT_PRIORITY	5
ADC_CLOCK_SETUP_T ADCSetup;			//Estructura de condiguracion del ADC
#define MAX_ADC_SAMPLES			10	//Cantidad de muestras del ADC que se promedian
#define ADC_SAMPLES_DELAY		(1/(portTICK_RATE_MS))	//Delay entre actualizaciones de la temperatura

/* Pin de deteccion de fase */
#define PHASE_SCU_INT_PORT	6			//Puerto del SCU usado para la deteccion de fase
#define PHASE_SCU_INT_PIN	1			//Pin del SCU usado para la deteccion de fase
#define PHASE_GPIO_INT_PORT	3			//Puerto del GPIO usado para la deteccion de fase
#define PHASE_GPIO_INT_PIN	0			//Pin del GPIO usado para la deteccion de fase
#define PHASE_PININT_INDEX	0			//Canal de las interrupciones para el cruce por cero
#define PHASE_INTERRUPT_PRIORITY	5	//Prioridad de la interrupcion del detector de cruce por cero
#define PHASE_IRQN_HANDLER	GPIO0_IRQHandler	//Handler de la interrupcion
#define PHASE_IRQN			PIN_INT0_IRQn		//Interrupcion

/* Pin de trigger del triac */
#define TRIGGER_SCU_INT_PORT	6			//Puerto del SCU usado para el trigger del triac
#define TRIGGER_SCU_INT_PIN		5			//Pin del SCU usado para el trigger del triac
#define TRIGGER_GPIO_INT_PORT	3			//Puerto del GPIO usado para el trigger del triac
#define TRIGGER_GPIO_INT_PIN	4			//Pin del GPIO usado para el trigger del triac

/* Pin del triac para el cooler */
#define COOLER_SCU_INT_PORT		6			//Puerto del SCU usado para el cooler
#define COOLER_SCU_INT_PIN		10			//Pin del SCU usado para el cooler
#define COOLER_GPIO_INT_PORT	3			//Puerto del GPIO usado para el cooler
#define COOLER_GPIO_INT_PIN		6			//Pin del GPIO usado para el cooler

/* Pin de encendido */
#define START_SCU_INT_PORT	6			//Puerto del SCU usado para el boton de encendido
#define START_SCU_INT_PIN	4			//Pin del SCU usado para el boton de encendido
#define START_GPIO_INT_PORT	3			//Puerto del GPIO usado para el boton de encendido
#define START_GPIO_INT_PIN	3			//Pin del GPIO usado para el boton de encendido
#define START_PININT_INDEX	1			//Canal de las interrupciones para el boton de encendido
#define START_INTERRUPT_PRIORITY	6	//Prioridad de la interrupcion para el boton de encendido
#define START_IRQN_HANDLER	GPIO1_IRQHandler	//Handler de la interrupcion
#define START_IRQN			PIN_INT1_IRQn		//Interrupcion

/* Pin de apagado */
#define STOP_SCU_INT_PORT	6			//Puerto del SCU usado para el boton de apagado
#define STOP_SCU_INT_PIN	9			//Pin del SCU usado para el boton de apagado
#define STOP_GPIO_INT_PORT	3			//Puerto del GPIO usado para el boton de apagado
#define STOP_GPIO_INT_PIN	5			//Pin del GPIO usado para el boton de apagado
#define STOP_PININT_INDEX	2			//Canal de las interrupciones para el boton de apagado
#define STOP_INTERRUPT_PRIORITY	6		//Prioridad de la interrupcion para el boton de apagado
#define STOP_IRQN_HANDLER	GPIO2_IRQHandler	//Handler de la interrupcion
#define STOP_IRQN			PIN_INT2_IRQn		//Interrupcion

/* Pin de apagado de salida. Avisa que se apago automaticamente */
#define STOP_OUT_SCU_PORT	6			//Puerto del SCU usado para indicar el apagado automatico
#define STOP_OUT_SCU_PIN	11			//Pin del SCU usado para indicar el apagado automatico
#define STOP_OUT_GPIO_PORT	3			//Puerto del GPIO usado para indicar el apagado automatico
#define STOP_OUT_GPIO_PIN	7			//Pin del GPIO usado para indicar el apagado automatico

void SetupHardware(void);				//Configuracion del hardware
void ConfigureADC(void);				//Configuracion del ADC
void ConfigurePhaseDetector(void);		//Configuracion del detector de cruce por cero
void ConfigureTriggerResistor(void);	//Configuracion del trigger que controla la resistencia
void ConfigureStartButton(void);		//Configuracion del boton de encendido
void ConfigureStopButton(void);			//Configuracion del boton de apagado
void ConfigureCooler( void );			//Configuración del pin de control del cooler

#endif