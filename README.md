# TP_Final_Embebidos
El TP final de Seminario de Sistemas Embebidos consiste en realizar el control de temperatura para un horno SMD.

###################################################################################################
## Avances con el ADC
* Chip_ADC_Init: Hace una configuracion por default del ADC. Recibe LPC_AD0 o LPC_ADC1, y un puntero a una estructura
	del tipo ADC_CLOCK_SETUP_T que es la que contiene toda la informacion de la configuracion.	
* Chip_ADC_EnableChannel: Habilita un determinado canal del ADC. Recibe LPC_ADC0 o LPC_ADC1, el canal del ADC a utilizar (ADC_CH0-7).
* NVIC_EnableIRQ: Agrega las interrupciones del ADC al vector de interrupciones. Recibe ADC0_IRQn o ADC1_IRQn. 
	El handler es ADC0_IRQHandler o ADC1_IRQHandler.
* Chip_ADC_Int_SetChannelCmd: Habilita o desabilita las interrupciones de un terminado canal del ADC.
* Chip_ADC_SetBurstCmd: Este modo realiza conversiones a maxima velocidad. Apenas termina una conversión comienza otra. Por esta razón,
	si se desean leer los valores convertidos es necesario parar las conversiones en la interrupcion, ya que la velocidad de las
	conversiones no permiten realizar otras actividades (la interrupcion salta todo el tiempo). Este modo permite tambien hacer
	conversiones de todos los canales al mismo tiempo (primero convierte ADC0, luego ADC1 y asi. Convierte los canales activados),
	lo cual evita tener que multiplexar. Hay que tener cuidado porque las sucesivas conversiones se van pisando, asi que hay que 
	leerlas rapido.
* Chip_ADC_SetStartMode: Comienza una conversión del ADC. Permite seleccionar si la conversion quiere hacerse inmediatamente o con
	alguna señal de trigger.
	
### OBSERVACIONES: 
* Usando el StartMode, luego de cada conversión es necesario volver a habilitar el StartMode para realizar una nueva conversión.

###################################################################################################
## Avances con la salida PWM
El bloque PWM tiene 2 salidas (con polaridades opuestas), cada una con 3 canales: MCOA0/1/2 y MCOB0/1/2. Tiene tambien dos canales
de entrada que permiten generar un trigger.
Cada canal tiene un TC(Timer/Counter), el cual es comparado con un "Limit Register", y cuando coinciden, el TC se resetea de dos formas
diferentes:
* En el modo "Edge aligned Mode" el TC se resetea a cero.
* En el modo "Centered Mode" el TC invierte su sentido, y comienza a decrementarse hasta llegar a cero, donde vuelve a incremenetarse.
Cada canal tiene tambien un "Match Register", que tiene un valor menor al Limit Register:
* En el modo "Edge aligned Mode" las salidas se switchean siempre que se alcance el Match o el Limit Register.
* En el modo "Centered Mode" las salidas se switchean unicamente cuando se alcanza el Match Register.

Por lo tanto, el Limit Register controla el periodo mientras que el Match Register el duty cycle.
Al parecer esta libreria no esta implementada, pero lo que si esta es una adaptacion del SCT como PWM. En esta pagina hay información
para configurar el bloque SCT como PWM http://www.lpcware.com/content/faq/how-use-sct-standard-pwm-using-lpcopen. 
Para configurar este bloque se debe hacer:
1) Chip_SCTPWM_Init(LPC_SCT): Habilita un clock externo.
2) Chip_SCTPWM_SetRate(LPC_SCT, SCT_PWM_RATE): Se configura la frecuencia del SCT.
3) Se debe configurar el SCU de los pines a utilizar con Chip_SCU_PinMuxSet(). Las salidas del SCT estan conectadas a los pines 
	CTOUT_n (P4_2, P4_1, P2_10, P4_3, P2_12, P2_11, P1_2, P1_1,...).
4) Configurar el output pin correspondiente con Chip_SCTPWM_SetOutPin(LPC_SCT, index, pin), donde index es un numero que va de 1 a N,
	siendo N la cantidad de pines de salida o de Match Registers (el minimo de los dos), y pin es el pin de CTOUT a configurar.
5) Se configura el duty cycle con Chip_SCTPWM_SetDutyCycle(LPC_SCT, index, ticks), donde ticks indica la cantidad de ticks en que la 
	salida permanece encendida. Utilizando Chip_SCTPWM_PercentageToTicks(LPC_SCT, percent) se puede setear el duty cycle mediante
	porcentajes sobre el total.
6) Se inicia el PWM con Chip_SCTPWM_Start(LPC_SCT).
7) El PWM se detiene con Chip_SCTPWM_Stop(LPC_SCT).

### OBSERVACIONES:
* Chip_SCT_EnableEventInt(LPC_SCT_T *pSCT, CHIP_SCT_EVENT_T evt) permite habilitar las interrupciones de acuerdo a los eventos. El
	handler es SCT_IRQHandler(). Los posibles eventos son: match condition, input o output condition, combinaciones de ambos.
* El tema de las interrupciones no me funciono todavia, pero todo lo otro al parecer funciona perfectamente. Tendria que ver si logro
	hacer andar el pwm de la forma original.
		
###################################################################################################


















