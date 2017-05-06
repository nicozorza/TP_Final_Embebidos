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
-Chip_ADC_SetBurstCmd: Este modo realiza conversiones a maxima velocidad. Apenas termina una conversión comienza otra. Por esta razón,
	si se desean leer los valores convertidos es necesario parar las conversiones en la interrupcion, ya que la velocidad de las
	conversiones no permiten realizar otras actividades (la interrupcion salta todo el tiempo). Este modo permite tambien hacer
	conversiones de todos los canales al mismo tiempo (primero convierte ADC0, luego ADC1 y asi. Convierte los canales activados),
	lo cual evita tener que multiplexar. Hay que tener cuidado porque las sucesivas conversiones se van pisando, asi que hay que 
	leerlas rapido.
-Chip_ADC_SetStartMode: Comienza una conversión del ADC. Permite seleccionar si la conversion quiere hacerse inmediatamente o con
	alguna señal de trigger.
	
#OBSERVACION: 
-Usando el StartMode, luego de cada conversión es necesario volver a habilitar el StartMode para realizar una nueva conversión.
######################################################################################################################################
