#include "utils.h"

/* Funcion que mapea la temperatura a partir de la tension de la termocupla */
float get_temperature( uint16_t adc_value ){
	float temp=0;

	temp = ((float)adc_value)*3.3/1024;		//Lo mapeo de 0-3.3v
	temp /= OPAMP_GAIN;						//Lo divido por la ganancia del operacional
	temp = 23701.6*temp-8.75; //23376.525*temp-1.574;				//Mapeo la temperatura
return temp;
}

/* Funcion que aplica el controlador a los datos medidios.
 * Se retorna la cantidad de semiciclos que permanece encendido el trigger */
int controller( float temperature, float reference ){
	float aux=0;

	aux=( reference-temperature )*KP;
	aux*=MAX_CYCLES_COUNT;		//Se mapea como un porcentaje de la cantidad maxima de semiciclos

	if( aux < 0 )	//Si es negativo es porque se paso la temperatura
			aux=0;
	if( aux >= MAX_CYCLES_COUNT)	//No puedo tener mas ciclos que el maximo
		aux=MAX_CYCLES_COUNT;

	return (int)(aux);	//Retorno la cantidad de semiciclos que debe estar encendido el control
}
