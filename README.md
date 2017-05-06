# TP_Final_Embebidos
El TP final de Seminario de Sistemas Embebidos consiste en realizar el control de temperatura para un horno SMD.

######################################################################################################################################
# Avances con el ADC
-Chip_ADC_Init: Hace una configuracion por default del ADC. Recibe LPC_AD0 o LPC_ADC1, y un puntero a una estructura
	del tipo ADC_CLOCK_SETUP_T que es la que contiene toda la informacion de la configuracion.	
-Chip_ADC_EnableChannel: Habilita un determinado canal del ADC. Recibe LPC_ADC0 o LPC_ADC1, el canal del ADC a utilizar (ADC_CH0-7).
-NVIC_EnableIRQ: Agrega las interrupciones del ADC al vector de interrupciones. Recibe ADC0_IRQn o ADC1_IRQn. 
	El handler es ADC0_IRQHandler o ADC1_IRQHandler.
-Chip_ADC_Int_SetChannelCmd: Habilita o desabilita las interrupciones de un terminado canal del ADC.
-Chip_ADC_SetBurstCmd: Hace una conversion en Burst Mode, pero no termino de entender como funciona. Creo que para lo que quiero hacer
	no funciona. 
-Chip_ADC_SetStartMode: Comienza una conversión del ADC. Permite seleccionar si la conversion quiere hacerse inmediatamente o con
	alguna señal de trigger.
	
#OBSERVACION: 
-Usando el StartMode, luego de cada conversión es necesario volver a habilitar el StartMode para realizar una nueva conversión.
######################################################################################################################################
