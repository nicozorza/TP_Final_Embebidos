#ifndef UTILS_H_
#define UTILS_H_

#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include <stdlib.h>

float get_temperature(uint16_t adc_value);	//Funcion que mapea la tension de la termocupla y la temperatura
int controller( float temperature, float reference );	//Funcion que aplica el controlador

#define MAX_CYCLES_COUNT	10		//Cantidad maxima de ciclos para el control por ciclos enteros

#define OPAMP_GAIN	156		//Ganancia del circuito amplificador de la termocupla

#define KP	1/5				//Inversa de la ganancia del control proporcional

#endif
