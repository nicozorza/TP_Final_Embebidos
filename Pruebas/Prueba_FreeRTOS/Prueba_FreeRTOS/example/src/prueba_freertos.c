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

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
#define EXAMPLE_10 (10)		/* Blocking when receiving from a queue */
#define EXAMPLE_11 (11)		/* Blocking when sending to a queue or sending structures on a queue */
#define EXAMPLE_12 (12)		/* Using a binary semaphore to synchronize a task with an interrupt */
#define EXAMPLE_13 (13)		/* Using a counting semaphore to synchronize a task with an interrupt */
#define EXAMPLE_14 (14)		/* Sending and receiving on a queue from within an interrupt */
#define EXAMPLE_15 (15)		/* Re-writing vPrintString() to use a semaphore */
#define EXAMPLE_16 (16)		/* Re-writing vPrintString() to use a gatekeeper task */

#define TEST (EXAMPLE_16)

#if (TEST == EXAMPLE_10)		/* Blocking when receiving from a queue */

const char *pcTextForMain = "\r\nExample 10 - Blocking when receiving from a queue\r\n";

/* The tasks to be created.  Two instances are created of the sender task while
 * only a single instance is created of the receiver task. */
static void vSenderTask(void *pvParameters);
static void vReceiverTask(void *pvParameters);

/* Declare a variable of type xQueueHandle.  This is used to store the queue
 * that is accessed by all three tasks. */
xQueueHandle xQueue;

/* Sender, UART (or output) & LED ON thread */
static void vSenderTask(void *pvParameters)
{
	long lValueToSend;
	portBASE_TYPE xStatus;

	/* Two instances are created of this task so the value that is sent to the
	 * queue is passed in via the task parameter rather than be hard coded.  This way
	 * each instance can use a different value.  Cast the parameter to the required
	 * 	type. */
	lValueToSend = (long) pvParameters;

	/* As per most tasks, this task is implemented in an infinite loop. */
	while (1) {
		Board_LED_Set(LED3, LED_ON);

		/* The first parameter is the queue to which data is being sent.  The
		 * queue was created before the scheduler was started, so before this task
		 * started to execute.
		 *
		 * The second parameter is the address of the data to be sent.
		 *
		 * The third parameter is the Block time � the time the task should be kept
		 * in the Blocked state to wait for space to become available on the queue
		 * should the queue already be full.  In this case we don�t specify a block
		 * time because there should always be space in the queue. */
		xStatus = xQueueSendToBack(xQueue, &lValueToSend, (portTickType)0);

		if (xStatus != pdPASS) {
			/* We could not write to the queue because it was full � this must
			 * be an error as the queue should never contain more than one item! */
			DEBUGOUT("Could not send to the queue.\r\n");
		}

		/* Allow the other sender task to execute. */
		taskYIELD();
	}
}

/* Receiver, UART (or output) & LED OFF thread */
static void vReceiverTask(void *pvParameters)
{
	/* Declare the variable that will hold the values received from the queue. */
	long lReceivedValue;
	portBASE_TYPE xStatus;
	const portTickType xTicksToWait = 100 / portTICK_RATE_MS;


	while (1) {
		Board_LED_Set(LED3, LED_OFF);

		/* As this task unblocks immediately that data is written to the queue this
		 * call should always find the queue empty. */
		if (uxQueueMessagesWaiting(xQueue) != 0) {
			DEBUGOUT("Queue should have been empty!\r\n");
		}

		/* The first parameter is the queue from which data is to be received.  The
		 * queue is created before the scheduler is started, and therefore before this
		 * task runs for the first time.
		 *
		 * The second parameter is the buffer into which the received data will be
		 * placed.  In this case the buffer is simply the address of a variable that
		 * has the required size to hold the received data.
		 *
		 * The last parameter is the block time � the maximum amount of time that the
		 * task should remain in the Blocked state to wait for data to be available should
		 * the queue already be empty. */
		xStatus = xQueueReceive(xQueue, &lReceivedValue, xTicksToWait);

		if (xStatus == pdPASS) {
			/* Data was successfully received from the queue, print out the received
			 * value. */
			DEBUGOUT("Received = %d\r\n", lReceivedValue);
		}
		else {
			/* We did not receive anything from the queue even after waiting for 100ms.
			 * This must be an error as the sending tasks are free running and will be
			 * continuously writing to the queue. */
			DEBUGOUT("Could not receive from the queue.\r\n");
		}
	}
}


/*****************************************************************************
 * Public functions
 ****************************************************************************/
/**
 * @brief	main routine for FreeRTOS example 10 - Blocking when receiving from a queue
 * @return	Nothing, function should not exit
 */
int main(void)
{
	/* Sets up system hardware */
	prvSetupHardware();

	/* Print out the name of this example. */
	DEBUGOUT(pcTextForMain);

    /* The queue is created to hold a maximum of 5 long values. */
    xQueue = xQueueCreate(5, sizeof(long));

	if (xQueue != (xQueueHandle)NULL) {
		/* Create two instances of the task that will write to the queue.  The
		 * parameter is used to pass the value that the task should write to the queue,
		 * so one task will continuously write 100 to the queue while the other task
		 * will continuously write 200 to the queue.  Both tasks are created at
		 * priority 1. */
		xTaskCreate(vSenderTask, (char *) "vSender1", configMINIMAL_STACK_SIZE, (void *) 100,
					(tskIDLE_PRIORITY + 1UL), (xTaskHandle *) NULL);
		xTaskCreate(vSenderTask, (char *) "vSender2", configMINIMAL_STACK_SIZE, (void *) 200,
					(tskIDLE_PRIORITY + 1UL), (xTaskHandle *) NULL);

		/* Create the task that will read from the queue.  The task is created with
		 * priority 2, so above the priority of the sender tasks. */
		xTaskCreate(vReceiverTask, (char *) "vReceiver", configMINIMAL_STACK_SIZE, NULL,
					(tskIDLE_PRIORITY + 2UL), (xTaskHandle *) NULL);

	/* Start the scheduler */
	vTaskStartScheduler();
	}
	else {
		/* The queue could not be created. */
	}

    /* If all is well we will never reach here as the scheduler will now be
     * running the tasks.  If we do reach here then it is likely that there was
     * insufficient heap memory available for a resource to be created. */
	while (1);

	/* Should never arrive here */
	return ((int) NULL);
}
#endif


#if (TEST == EXAMPLE_11)		/* Blocking when sending to a queue or sending structures on a queue */

const char *pcTextForMain = "\r\nExample 10 - Blocking when sending to a queue or sending structures on a queue\r\n";

#define mainSENDER_1		1
#define mainSENDER_2		2

/* The tasks to be created.  Two instances are created of the sender task while
 * only a single instance is created of the receiver task. */
static void vSenderTask(void *pvParameters);
static void vReceiverTask(void *pvParameters);

/* Declare a variable of type xQueueHandle.  This is used to store the queue
 * that is accessed by all three tasks. */
xQueueHandle xQueue;

/* Define the structure type that will be passed on the queue. */
typedef struct {
	unsigned char ucValue;
	unsigned char ucSource;
} xData;

/* Declare two variables of type xData that will be passed on the queue. */
static const xData xStructsToSend[2] = {
	{100, mainSENDER_1}, /* Used by Sender1. */
	{200, mainSENDER_2}  /* Used by Sender2. */
};


/* Sender, UART (or output) & LED ON thread */
static void vSenderTask(void *pvParameters)
{
	portBASE_TYPE xStatus;
	const portTickType xTicksToWait = 100 / portTICK_RATE_MS;

	/* As per most tasks, this task is implemented in an infinite loop. */
	while (1) {
		Board_LED_Set(LED3, LED_ON);

		/* The first parameter is the queue to which data is being sent.  The
		 * queue was created before the scheduler was started, so before this task
		 * started to execute.
		 *
		 * The second parameter is the address of the structure being sent.  The
		 * address is passed in as the task parameter.
		 *
		 * The third parameter is the Block time - the time the task should be kept
		 * in the Blocked state to wait for space to become available on the queue
		 * should the queue already be full.  A block time is specified as the queue
		 * will become full.  Items will only be removed from the queue when both
		 * sending tasks are in the Blocked state.. */
		xStatus = xQueueSendToBack(xQueue, pvParameters, xTicksToWait);

		if (xStatus != pdPASS) {
			/* We could not write to the queue because it was full - this must
			 * be an error as the receiving task should make space in the queue
			 * as soon as both sending tasks are in the Blocked state. */
			DEBUGOUT("Could not send to the queue.\r\n");
		}

		/* Allow the other sender task to execute. */
		taskYIELD();
	}
}

/* Receiver, UART (or output) & LED OFF thread */
static void vReceiverTask(void *pvParameters)
{
	/* Declare the structure that will hold the values received from the queue. */
	xData xReceivedStructure;
	portBASE_TYPE xStatus;

	/* This task is also defined within an infinite loop. */
	while (1) {
		Board_LED_Set(LED3, LED_OFF);

		/* As this task only runs when the sending tasks are in the Blocked state,
		 * and the sending tasks only block when the queue is full, this task should
		 * always find the queue to be full.  3 is the queue length. */
		if (uxQueueMessagesWaiting(xQueue) != 3) {
			DEBUGOUT("Queue should have been full!\r\n");
		}

		/* The first parameter is the queue from which data is to be received.  The
		 * queue is created before the scheduler is started, and therefore before this
		 * task runs for the first time.
		 *
		 * The second parameter is the buffer into which the received data will be
		 * placed.  In this case the buffer is simply the address of a variable that
		 * has the required size to hold the received structure.
		 *
		 * The last parameter is the block time - the maximum amount of time that the
		 * task should remain in the Blocked state to wait for data to be available
		 * should the queue already be empty.  A block time is not necessary as this
		 * task will only run when the queue is full so data will always be available. */
		xStatus = xQueueReceive(xQueue, &xReceivedStructure, (portBASE_TYPE)0);

		if (xStatus == pdPASS) {
			/* Data was successfully received from the queue, print out the received
			 * value and the source of the value. */
			if (xReceivedStructure.ucSource == mainSENDER_1) {
				DEBUGOUT("From Sender 1 = %d\r\n", xReceivedStructure.ucValue);
			}
			else {
				DEBUGOUT("From Sender 2 = %d\r\n", xReceivedStructure.ucValue);
			}
		}
		else {
			/* We did not receive anything from the queue.  This must be an error
			 * as this task should only run when the queue is full. */
			DEBUGOUT("Could not receive from the queue.\r\n");
		}
	}
}


/*****************************************************************************
 * Public functions
 ****************************************************************************/
/**
 * @brief	main routine for FreeRTOS example 11 - Blocking when sending to a queue or sending structures on a queue
 * @return	Nothing, function should not exit
 */
int main(void)
{
	/* Sets up system hardware */
	prvSetupHardware();

	/* Print out the name of this example. */
	DEBUGOUT(pcTextForMain);

    /* The queue is created to hold a maximum of 3 structures of type xData. */
    xQueue = xQueueCreate(3, sizeof(xData));

	if (xQueue != (xQueueHandle)NULL) {
		/* Create two instances of the task that will write to the queue.  The
		 * parameter is used to pass the structure that the task should write to the
		 * queue, so one task will continuously send xStructsToSend[ 0 ] to the queue
		 * while the other task will continuously send xStructsToSend[ 1 ].  Both
		 * tasks are created at priority 2 which is above the priority of the receiver. */
		xTaskCreate(vSenderTask, (char *) "vSender1", configMINIMAL_STACK_SIZE, (void *) &(xStructsToSend[0]),
					(tskIDLE_PRIORITY + 2UL), (xTaskHandle *) NULL);
		xTaskCreate(vSenderTask, (char *) "vSender2", configMINIMAL_STACK_SIZE, (void *) &(xStructsToSend[1]),
					(tskIDLE_PRIORITY + 2UL), (xTaskHandle *) NULL);

		/* Create the task that will read from the queue.  The task is created with
		 * priority 1, so below the priority of the sender tasks. */
		xTaskCreate(vReceiverTask, (char *) "vReceiver", configMINIMAL_STACK_SIZE, NULL,
					(tskIDLE_PRIORITY + 1UL), (xTaskHandle *) NULL);

	/* Start the scheduler */
	vTaskStartScheduler();
	}
	else {
		/* The queue could not be created. */
	}

    /* If all is well we will never reach here as the scheduler will now be
     * running the tasks.  If we do reach here then it is likely that there was
     * insufficient heap memory available for a resource to be created. */
	while (1);

	/* Should never arrive here */
	return ((int) NULL);
}
#endif


#if (TEST == EXAMPLE_12)		/* Using a counting semaphore to synchronize a task with an interrupt */

const char *pcTextForMain = "\r\nExample 12 - Using a counting semaphore to synchronize a task with an interrupt\r\n";

/* The interrupt number to use for the software interrupt generation.  This
 * could be any unused number.  In this case the first chip level (non system)
 * interrupt is used, which happens to be the watchdog on the LPC1768.  WDT_IRQHandler */
#define mainSW_INTERRUPT_ID		(0)

/* Macro to force an interrupt. */
#define mainTRIGGER_INTERRUPT()	NVIC_SetPendingIRQ(mainSW_INTERRUPT_ID)

/* Macro to clear the same interrupt. */
#define mainCLEAR_INTERRUPT()	NVIC_ClearPendingIRQ(mainSW_INTERRUPT_ID)

/* The priority of the software interrupt.  The interrupt service routine uses
 * an (interrupt safe) FreeRTOS API function, so the priority of the interrupt must
 * be equal to or lower than the priority set by
 * configMAX_SYSCALL_INTERRUPT_PRIORITY - remembering that on the Cortex-M3 high
 * numeric values represent low priority values, which can be confusing as it is
 * counter intuitive. */
#define mainSOFTWARE_INTERRUPT_PRIORITY	(5)

/* The tasks to be created. */
static void vHandlerTask(void *pvParameters);
static void vPeriodicTask(void *pvParameters);

/* Enable the software interrupt and set its priority. */
static void prvSetupSoftwareInterrupt();

/* The service routine for the interrupt.  This is the interrupt that the
 * task will be synchronized with.  void vSoftwareInterruptHandler(void); */
#define vSoftwareInterruptHandler (WDT_IRQHandler)

/* Declare a variable of type xSemaphoreHandle.  This is used to reference the
 * semaphore that is used to synchronize a task with an interrupt. */
xSemaphoreHandle xBinarySemaphore;


/* Take Semaphore, UART (or output) & LED toggle thread */
static void vHandlerTask(void *pvParameters)
{
	/* As per most tasks, this task is implemented within an infinite loop.
	 *
	 * Take the semaphore once to start with so the semaphore is empty before the
	 * infinite loop is entered.  The semaphore was created before the scheduler
	 * was started so before this task ran for the first time.*/
    xSemaphoreTake(xBinarySemaphore, (portTickType) 0);

	while (1) {
		Board_LED_Toggle(LED3);

		/* Use the semaphore to wait for the event.  The task blocks
         * indefinitely meaning this function call will only return once the
         * semaphore has been successfully obtained - so there is no need to check
         * the returned value. */
        xSemaphoreTake(xBinarySemaphore, portMAX_DELAY);

        /* To get here the event must have occurred.  Process the event (in this
         * case we just print out a message). */
        DEBUGOUT("Handler task - Processing event.\r\n");
    }
}


static void vPeriodicTask(void *pvParameters)
{
    /* As per most tasks, this task is implemented within an infinite loop. */
	while (1) {
		/* This task is just used to 'simulate' an interrupt.  This is done by
         * periodically generating a software interrupt. */
        vTaskDelay(500 / portTICK_RATE_MS);

        /* Generate the interrupt, printing a message both before hand and
         * afterwards so the sequence of execution is evident from the output. */
        DEBUGOUT("Periodic task - About to generate an interrupt.\r\n");
        mainTRIGGER_INTERRUPT();
        DEBUGOUT("Periodic task - Interrupt generated.\n\n");
    }
}


static void prvSetupSoftwareInterrupt()
{
	/* The interrupt service routine uses an (interrupt safe) FreeRTOS API
	 * function so the interrupt priority must be at or below the priority defined
	 * by configSYSCALL_INTERRUPT_PRIORITY. */
	NVIC_SetPriority(mainSW_INTERRUPT_ID, mainSOFTWARE_INTERRUPT_PRIORITY);

	/* Enable the interrupt. */
	NVIC_EnableIRQ(mainSW_INTERRUPT_ID);
}


void vSoftwareInterruptHandler(void)
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

    /* 'Give' the semaphore to unblock the task. */
    xSemaphoreGiveFromISR(xBinarySemaphore, &xHigherPriorityTaskWoken);

    /* Clear the software interrupt bit using the interrupt controllers
     * Clear Pending register. */
    mainCLEAR_INTERRUPT();

    /* Giving the semaphore may have unblocked a task - if it did and the
     * unblocked task has a priority equal to or above the currently executing
     * task then xHigherPriorityTaskWoken will have been set to pdTRUE and
     * portEND_SWITCHING_ISR() will force a context switch to the newly unblocked
     * higher priority task.
     *
     * NOTE: The syntax for forcing a context switch within an ISR varies between
     * FreeRTOS ports.  The portEND_SWITCHING_ISR() macro is provided as part of
     * the Cortex-M3 port layer for this purpose.  taskYIELD() must never be called
     * from an ISR! */
    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}


/*****************************************************************************
 * Public functions
 ****************************************************************************/
/**
 * @brief	main routine for FreeRTOS example 12 - Using a binary semaphore to synchronize a task with an interrupt
 * @return	Nothing, function should not exit
 */
int main(void)
{
	/* Sets up system hardware */
	prvSetupHardware();

	/* Print out the name of this example. */
	DEBUGOUT(pcTextForMain);

    /* Before a semaphore is used it must be explicitly created.  In this example
     * a binary semaphore is created. */
    vSemaphoreCreateBinary(xBinarySemaphore);

    /* Check the semaphore was created successfully. */
    if (xBinarySemaphore != (xSemaphoreHandle) NULL) {
    	/* Enable the software interrupt and set its priority. */
    	prvSetupSoftwareInterrupt();

        /* Create the 'handler' task.  This is the task that will be synchronized
         * with the interrupt.  The handler task is created with a high priority to
         * ensure it runs immediately after the interrupt exits.  In this case a
         * priority of 3 is chosen. */
        xTaskCreate(vHandlerTask, (char *) "Handler", configMINIMAL_STACK_SIZE, NULL,
        			(tskIDLE_PRIORITY + 3UL), (xTaskHandle *) NULL);

        /* Create the task that will periodically generate a software interrupt.
         * This is created with a priority below the handler task to ensure it will
         * get preempted each time the handler task exits the Blocked state. */
        xTaskCreate(vPeriodicTask, (char *) "Periodic", configMINIMAL_STACK_SIZE, NULL,
        			(tskIDLE_PRIORITY + 1UL), (xTaskHandle *) NULL);

        /* Start the scheduler so the created tasks start executing. */
        vTaskStartScheduler();
    }

    /* If all is well we will never reach here as the scheduler will now be
     * running the tasks.  If we do reach here then it is likely that there was
     * insufficient heap memory available for a resource to be created. */
	while (1);

	/* Should never arrive here */
    return ((int) NULL);
}

#endif


#if (TEST == EXAMPLE_13)		/* Using a binary semaphore to synchronize a task with an interrupt */

const char *pcTextForMain = "\r\nExample 13 - Using a binary semaphore to synchronize a task with an interrupt\r\n";

/* The interrupt number to use for the software interrupt generation.  This
 * could be any unused number.  In this case the first chip level (non system)
 * interrupt is used, which happens to be the watchdog on the LPC1768.  WDT_IRQHandler */
#define mainSW_INTERRUPT_ID		(0)

/* Macro to force an interrupt. */
#define mainTRIGGER_INTERRUPT()	NVIC_SetPendingIRQ(mainSW_INTERRUPT_ID)

/* Macro to clear the same interrupt. */
#define mainCLEAR_INTERRUPT()	NVIC_ClearPendingIRQ(mainSW_INTERRUPT_ID)

/* The priority of the software interrupt.  The interrupt service routine uses
 * an (interrupt safe) FreeRTOS API function, so the priority of the interrupt must
 * be equal to or lower than the priority set by
 * configMAX_SYSCALL_INTERRUPT_PRIORITY - remembering that on the Cortex-M3 high
 * numeric values represent low priority values, which can be confusing as it is
 * counter intuitive. */
#define mainSOFTWARE_INTERRUPT_PRIORITY	(5)

/* The tasks to be created. */
static void vHandlerTask(void *pvParameters);
static void vPeriodicTask(void *pvParameters);

/* Enable the software interrupt and set its priority. */
static void prvSetupSoftwareInterrupt();

/* The service routine for the interrupt.  This is the interrupt that the
* task will be synchronized with.  void vSoftwareInterruptHandler(void); */
#define vSoftwareInterruptHandler (WDT_IRQHandler)

/* Declare a variable of type xSemaphoreHandle.  This is used to reference the
 * semaphore that is used to synchronize a task with an interrupt. */
xSemaphoreHandle xCountingSemaphore;


/* Take Semaphore, UART (or output) & LED toggle thread */
static void vHandlerTask(void *pvParameters)
{
	/* As per most tasks, this task is implemented within an infinite loop. */
	while (1) {
		Board_LED_Toggle(LED3);

		/* Use the semaphore to wait for the event.  The semaphore was created
		 * before the scheduler was started so before this task ran for the first
		 * time.  The task blocks indefinitely meaning this function call will only
		 * return once the semaphore has been successfully obtained - so there is no
		 * need to check the returned value. */
		xSemaphoreTake(xCountingSemaphore, portMAX_DELAY);

		/* To get here the event must have occurred.  Process the event (in this
		 * case we just print out a message). */
		DEBUGOUT("Handler task - Processing event.\r\n");
    }
}


static void vPeriodicTask(void *pvParameters)
{
	/* As per most tasks, this task is implemented within an infinite loop. */
	while (1) {
		/* This task is just used to 'simulate' an interrupt.  This is done by
		 * periodically generating a software interrupt. */
		vTaskDelay(500 / portTICK_RATE_MS);

		/* Generate the interrupt, printing a message both before hand and
		 * afterwards so the sequence of execution is evident from the output. */
        DEBUGOUT("Periodic task - About to generate an interrupt.\r\n");
        mainTRIGGER_INTERRUPT();
        DEBUGOUT("Periodic task - Interrupt generated.\r\n");
    }
}


static void prvSetupSoftwareInterrupt()
{
	/* The interrupt service routine uses an (interrupt safe) FreeRTOS API
	 * function so the interrupt priority must be at or below the priority defined
	 * by configSYSCALL_INTERRUPT_PRIORITY. */
	NVIC_SetPriority(mainSW_INTERRUPT_ID, mainSOFTWARE_INTERRUPT_PRIORITY);

	/* Enable the interrupt. */
	NVIC_EnableIRQ(mainSW_INTERRUPT_ID);
}


void vSoftwareInterruptHandler(void)
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	/* 'Give' the semaphore multiple times.  The first will unblock the handler
	 * task, the following 'gives' are to demonstrate that the semaphore latches
	 * the events to allow the handler task to process them in turn without any
	 * events getting lost.  This simulates multiple interrupts being taken by the
	 * processor, even though in this case the events are simulated within a single
	 * interrupt occurrence.*/
	xSemaphoreGiveFromISR(xCountingSemaphore, &xHigherPriorityTaskWoken);
	xSemaphoreGiveFromISR(xCountingSemaphore, &xHigherPriorityTaskWoken);
	xSemaphoreGiveFromISR(xCountingSemaphore, &xHigherPriorityTaskWoken);

    /* Clear the software interrupt bit using the interrupt controllers
     * Clear Pending register. */
    mainCLEAR_INTERRUPT();

    /* Giving the semaphore may have unblocked a task - if it did and the
     * unblocked task has a priority equal to or above the currently executing
     * task then xHigherPriorityTaskWoken will have been set to pdTRUE and
     * portEND_SWITCHING_ISR() will force a context switch to the newly unblocked
     * higher priority task.
     *
     * NOTE: The syntax for forcing a context switch within an ISR varies between
     * FreeRTOS ports.  The portEND_SWITCHING_ISR() macro is provided as part of
     * the Cortex-M3 port layer for this purpose.  taskYIELD() must never be called
     * from an ISR! */
    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}


/*****************************************************************************
 * Public functions
 ****************************************************************************/
/**
 * @brief	main routine for FreeRTOS example 13 - Using a counting semaphore to synchronize a task with an interrupt
 * @return	Nothing, function should not exit
 */
int main(void)
{
	/* Sets up system hardware */
	prvSetupHardware();

	/* Print out the name of this example. */
	DEBUGOUT(pcTextForMain);

    /* Before a semaphore is used it must be explicitly created.  In this example
     * a counting semaphore is created.  The semaphore is created to have a maximum
     * count value of 10, and an initial count value of 0. */
    xCountingSemaphore = xSemaphoreCreateCounting(10, 0);

	/* Check the semaphore was created successfully. */
	if (xCountingSemaphore != NULL)
	{
    	/* Enable the software interrupt and set its priority. */
    	prvSetupSoftwareInterrupt();

		/* Create the 'handler' task.  This is the task that will be synchronized
		with the interrupt.  The handler task is created with a high priority to
		ensure it runs immediately after the interrupt exits.  In this case a
		priority of 3 is chosen. */
        xTaskCreate(vHandlerTask, (char *) "Handler", configMINIMAL_STACK_SIZE, NULL,
        			(tskIDLE_PRIORITY + 3UL), (xTaskHandle *) NULL);

		/* Create the task that will periodically generate a software interrupt.
		This is created with a priority below the handler task to ensure it will
		get preempted each time the handler task exist the Blocked state. */
        xTaskCreate(vPeriodicTask, (char *) "Periodic", configMINIMAL_STACK_SIZE, NULL,
        			(tskIDLE_PRIORITY + 1UL), (xTaskHandle *) NULL);

		/* Start the scheduler so the created tasks start executing. */
		vTaskStartScheduler();
	}

	/* If all is well we will never reach here as the scheduler will now be
     * running the tasks.  If we do reach here then it is likely that there was
     * insufficient heap memory available for a resource to be created. */
	while (1);

	/* Should never arrive here */
    return ((int) NULL);
}
#endif


#if (TEST == EXAMPLE_14)		/* Sending and receiving on a queue from within an interrupt */

const char *pcTextForMain = "\r\nExample 14 - Sending and receiving on a queue from within an interrupt\r\n";

/* The interrupt number to use for the software interrupt generation.  This
 * could be any unused number.  In this case the first chip level (non system)
 * interrupt is used, which happens to be the watchdog on the LPC1768.  WDT_IRQHandler */
#define mainSW_INTERRUPT_ID		(0)

/* Macro to force an interrupt. */
#define mainTRIGGER_INTERRUPT()	NVIC_SetPendingIRQ(mainSW_INTERRUPT_ID)

/* Macro to clear the same interrupt. */
#define mainCLEAR_INTERRUPT()	NVIC_ClearPendingIRQ(mainSW_INTERRUPT_ID)

/* The priority of the software interrupt.  The interrupt service routine uses
 * an (interrupt safe) FreeRTOS API function, so the priority of the interrupt must
 * be equal to or lower than the priority set by
 * configMAX_SYSCALL_INTERRUPT_PRIORITY - remembering that on the Cortex-M3 high
 * numeric values represent low priority values, which can be confusing as it is
 * counter intuitive. */
#define mainSOFTWARE_INTERRUPT_PRIORITY	(5)

/* The tasks to be created. */
static void vIntegerGenerator(void *pvParameters);
static void vStringPrinter(void *pvParameters);

/* Enable the software interrupt and set its priority. */
static void prvSetupSoftwareInterrupt();

/* The service routine for the interrupt.  This is the interrupt that the
* task will be synchronized with.  void vSoftwareInterruptHandler(void); */
#define vSoftwareInterruptHandler (WDT_IRQHandler)

unsigned long ulNext = 0;
unsigned long ulCount;
unsigned long ul[100];

/* Declare two variables of type xQueueHandle.  One queue will be read from
 * within an ISR, the other will be written to from within an ISR. */
xQueueHandle xIntegerQueue, xStringQueue;


/* Take Semaphore, UART (or output) & LED toggle thread */
static void vIntegerGenerator(void *pvParameters)
{
	portTickType xLastExecutionTime;
	unsigned portLONG ulValueToSend = 0;
	int i;

	/* Initialize the variable used by the call to vTaskDelayUntil(). */
	xLastExecutionTime = xTaskGetTickCount();

	/* As per most tasks, this task is implemented within an infinite loop. */
	while (1) {
		Board_LED_Toggle(LED3);

		/* This is a periodic task.  Block until it is time to run again.
		 * The task will execute every 200ms. */
		vTaskDelayUntil(&xLastExecutionTime, 200 / portTICK_RATE_MS);

		/* Send an incrementing number to the queue five times.  These will be
		 * read from the queue by the interrupt service routine.  A block time is
		 * not specified. */
		for (i = 0; i < 5; i++) {
			xQueueSendToBack(xIntegerQueue, &ulValueToSend, 0);
			ulValueToSend++;
		}

		/* Force an interrupt so the interrupt service routine can read the
		 * values from the queue. */
		DEBUGOUT("Generator task - About to generate an interrupt.\r\n");
		mainTRIGGER_INTERRUPT();
		DEBUGOUT("Generator task - Interrupt generated.\r\n");
    }
}


static void vStringPrinter(void *pvParameters)
{
	char *pcString;

	/* As per most tasks, this task is implemented within an infinite loop. */
	while (1) {
		/* Block on the queue to wait for data to arrive. */
		xQueueReceive(xStringQueue, &pcString, portMAX_DELAY);

		/* Print out the string received. */
        DEBUGOUT(pcString);
    }
}


static void prvSetupSoftwareInterrupt()
{
	/* The interrupt service routine uses an (interrupt safe) FreeRTOS API
	 * function so the interrupt priority must be at or below the priority defined
	 * by configSYSCALL_INTERRUPT_PRIORITY. */
	NVIC_SetPriority(mainSW_INTERRUPT_ID, mainSOFTWARE_INTERRUPT_PRIORITY);

	/* Enable the interrupt. */
	NVIC_EnableIRQ(mainSW_INTERRUPT_ID);
}


void vSoftwareInterruptHandler(void)
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	static unsigned long ulReceivedNumber;

	/* The strings are declared static const to ensure they are not allocated to the
	 * interrupt service routine stack, and exist even when the interrupt service routine
	 * is not executing. */
	static const char *pcStrings[] = {
	    "String 0\r\n",
	    "String 1\r\n",
	    "String 2\r\n",
	    "String 3\r\n"
	};

    /* Loop until the queue is empty. */
    while (xQueueReceiveFromISR(xIntegerQueue, &ulReceivedNumber, &xHigherPriorityTaskWoken) != errQUEUE_EMPTY) {
        /* Truncate the received value to the last two bits (values 0 to 3 inc.), then
         * send the string that corresponds to the truncated value to the other
         * queue. */
        ulReceivedNumber &= 0x03;
        xQueueSendToBackFromISR(xStringQueue, &pcStrings[ulReceivedNumber], &xHigherPriorityTaskWoken);
    }

    /* Clear the software interrupt bit using the interrupt controllers
     * Clear Pending register. */
    mainCLEAR_INTERRUPT();

    /* xHigherPriorityTaskWoken was initialised to pdFALSE.  It will have then
     * been set to pdTRUE only if reading from or writing to a queue caused a task
     * of equal or greater priority than the currently executing task to leave the
     * Blocked state.  When this is the case a context switch should be performed.
     * In all other cases a context switch is not necessary.
     *
     * NOTE: The syntax for forcing a context switch within an ISR varies between
     * FreeRTOS ports.  The portEND_SWITCHING_ISR() macro is provided as part of
     * the Cortex-M3 port layer for this purpose.  taskYIELD() must never be called
     * from an ISR! */
    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}


/*****************************************************************************
 * Public functions
 ****************************************************************************/
/**
 * @brief	main routine for FreeRTOS example 14 - Sending and receiving on a queue from within an interrupt
 * @return	Nothing, function should not exit
 */
int main(void)
{
	/* Sets up system hardware */
	prvSetupHardware();

	/* Print out the name of this example. */
	DEBUGOUT(pcTextForMain);

    /* Before a queue can be used it must first be created.  Create both queues
     * used by this example.  One queue can hold variables of type unsigned long,
     * the other queue can hold variables of type char*.  Both queues can hold a
     * maximum of 10 items.  A real application should check the return values to
     * ensure the queues have been successfully created. */
    xIntegerQueue = xQueueCreate(10, sizeof(unsigned long));
	xStringQueue = xQueueCreate(10, sizeof(char *));

   	/* Enable the software interrupt and set its priority. */
   	prvSetupSoftwareInterrupt();

	/* Create the task that uses a queue to pass integers to the interrupt service
	routine.  The task is created at priority 1. */
   	xTaskCreate(vIntegerGenerator, (char *) "IntGen", configMINIMAL_STACK_SIZE, NULL,
   				(tskIDLE_PRIORITY + 1UL), (xTaskHandle *) NULL);

    /* Create the task that prints out the strings sent to it from the interrupt
     * service routine.  This task is created at the higher priority of 2. */
    xTaskCreate(vStringPrinter, (char *) "String", configMINIMAL_STACK_SIZE, NULL,
        			(tskIDLE_PRIORITY + 2UL), (xTaskHandle *) NULL);

	/* Start the scheduler so the created tasks start executing. */
	vTaskStartScheduler();

	/* If all is well we will never reach here as the scheduler will now be
     * running the tasks.  If we do reach here then it is likely that there was
     * insufficient heap memory available for a resource to be created. */
	while (1);

	/* Should never arrive here */
    return ((int) NULL);
}
#endif


#if (TEST == EXAMPLE_15)		/* Re-writing vPrintString() to use a semaphore */

const char *pcTextForMain = "\r\nExample 15 - Re-writing vPrintString() to use a semaphore\r\n";

/* Dimensions the buffer into which messages destined for stdout are placed. */
#define mainMAX_MSG_LEN	( 80 )

/* The task to be created.  Two instances of this task are created. */
static void prvPrintTask(void *pvParameters);

/* The function that uses a mutex to control access to standard out. */
static void prvNewPrintString(const portCHAR *pcString);

/* Declare a variable of type xSemaphoreHandle.  This is used to reference the
 * mutex type semaphore that is used to ensure mutual exclusive access to stdout. */
xSemaphoreHandle xMutex;


/* Take Mutex Semaphore, UART (or output) & LED toggle thread */
static void prvNewPrintString(const portCHAR *pcString)
{
	static char cBuffer[mainMAX_MSG_LEN];

	/* The semaphore is created before the scheduler is started so already
	 * exists by the time this task executes.
	 *
	 * Attempt to take the semaphore, blocking indefinitely if the mutex is not
	 * available immediately.  The call to xSemaphoreTake() will only return when
	 * the semaphore has been successfully obtained so there is no need to check the
	 * return value.  If any other delay period was used then the code must check
	 * that xSemaphoreTake() returns pdTRUE before accessing the resource (in this
	 * case standard out. */
	xSemaphoreTake(xMutex, portMAX_DELAY);
	{
		/* The following line will only execute once the semaphore has been
		 * successfully obtained - so standard out can be accessed freely. */
		sprintf(cBuffer, "%s", pcString);
		DEBUGOUT(cBuffer);
	}
	xSemaphoreGive(xMutex);
}


static void prvPrintTask( void *pvParameters )
{
char *pcStringToPrint;

	/* Two instances of this task are created so the string the task will send
	 * to prvNewPrintString() is passed in the task parameter.  Cast this to the
	 * required type. */
	pcStringToPrint = (char *) pvParameters;

	while (1) {
		/* Print out the string using the newly defined function. */
		prvNewPrintString( pcStringToPrint );

		/* Wait a pseudo random time.  Note that rand() is not necessarily
		 * re-entrant, but in this case it does not really matter as the code does
		 * not care what value is returned.  In a more secure application a version
		 * of rand() that is known to be re-entrant should be used - or calls to
		 * rand() should be protected using a critical section. */
		vTaskDelay((rand() & 0x1FF));
	}
}


/*****************************************************************************
 * Public functions
 ****************************************************************************/
/**
 * @brief	main routine for FreeRTOS example 15 - Re-writing vPrintString() to use a semaphore
 * @return	Nothing, function should not exit
 */
int main(void)
{
	/* Sets up system hardware */
	prvSetupHardware();

	/* Print out the name of this example. */
	DEBUGOUT(pcTextForMain);

    /* Before a semaphore is used it must be explicitly created.  In this example
     * a mutex type semaphore is created. */
    xMutex = xSemaphoreCreateMutex();

	/* The tasks are going to use a pseudo random delay, seed the random number
	 * generator. */
	srand(567);

	/* Only create the tasks if the semaphore was created successfully. */
	if (xMutex != NULL) {
		/* Create two instances of the tasks that attempt to write stdout.  The
		 * string they attempt to write is passed in as the task parameter.  The tasks
		 * are created at different priorities so some pre-emption will occur. */
		xTaskCreate(prvPrintTask, (char *) "Print1", configMINIMAL_STACK_SIZE,
					"Task 1 ******************************************\r\n",
					(tskIDLE_PRIORITY + 1UL), (xTaskHandle *) NULL);
		xTaskCreate(prvPrintTask, (char *) "Print2", configMINIMAL_STACK_SIZE,
					"Task 2 ------------------------------------------\r\n",
					(tskIDLE_PRIORITY + 2UL), (xTaskHandle *) NULL);

		/* Start the scheduler so the created tasks start executing. */
		vTaskStartScheduler();
	}
	/* If all is well we will never reach here as the scheduler will now be
     * running the tasks.  If we do reach here then it is likely that there was
     * insufficient heap memory available for a resource to be created. */
	while (1);

	/* Should never arrive here */
    return ((int) NULL);
}
#endif


#if (TEST == EXAMPLE_16)		/* Re-writing vPrintString() to use a gatekeeper task */

/****************************************************************************
 * Prototipos de funciones
 ****************************************************************************/
void SetupHardware(void);				//Configuracion del hardware
void ConfigureADC( void );				//Configuracion del ADC
void ConfigurePhaseDetector( void );	//Configuracion del detector de cruce por cero
void ConfigureTimer( void );			//Configuracion del timer, el cual se usa para el delay del trigger
static void vHandlerZeroCrossing(void *pvParameters);	//Tarea de deteccion de cruce por cero
static void vHandlerFireTrigger(void *pvParameters);	//Tarea que dispara el trigger
/************************************************************/

/*************************************************************************
 * Variables gloables
 *************************************************************************/
xSemaphoreHandle xPhaseSemaphore;	//Semaforo para la deteccion de cruce por cero
xSemaphoreHandle xTimerSemaphore;	//Semaforo para la activacion del timer

volatile uint16_t adc_data;			//Valor leido en el ADC
ADC_CLOCK_SETUP_T ADCSetup;			//Estructura de condiguracion del ADC
#define ADC_INTERRUPT_PRIORITY	5	//Prioridad de la interrupcion del ADC

/* Pin de deteccion de fase */
#define PHASE_SCU_INT_PORT	1			//Puerto del SCU usado para la deteccion de fase
#define PHASE_SCU_INT_PIN		0			//Pin del SCU usado para la deteccion de fase
#define PHASE_GPIO_INT_PORT	0			//Puerto del GPIO usado para la deteccion de fase
#define PHASE_GPIO_INT_PIN	4			//Pin del GPIO usado para la deteccion de fase
#define PHASE_PININT_INDEX	0			//Canal de las interrupciones para el cruce por cero
#define PHASE_INTERRUPT_PRIORITY	5		//Prioridad de la interrupcion del detector de cruce por cero

/* Pin de trigger del triac */
#define TRIGGER_SCU_INT_PORT	6			//Puerto del SCU usado para el trigger del triac
#define TRIGGER_SCU_INT_PIN		5			//Pin del SCU usado para el trigger del triac
#define TRIGGER_GPIO_INT_PORT	3			//Puerto del GPIO usado para el trigger del triac
#define TRIGGER_GPIO_INT_PIN	4			//Pin del GPIO usado para el trigger del triac

/* Macros para el timer. Dado que el detector de cruce por cero no es inmediato, se deben
 * los margenes para evitar problemas. */
#define TIMER_BASE 100000
#define TIMER_MAX 500000
#define TIMER_STEP (TIMER_MAX-TIMER_BASE)/1024
#define TIMER_INTERRUPT_PRIORITY 5
/*****************************************************************************/

/*****************************************************************************
 * Otras funciones
*****************************************************************************/
/*
 * Uso el Hook de la tarea Idle. De esta forma tomo muestras del ADC siempre que haya
 * un intervalo de tiempo disponible.
 */
void vApplicationIdleHook(void){
	Chip_ADC_SetStartMode(LPC_ADC0, ADC_START_NOW, ADC_TRIGGERMODE_RISING);
}
/*****************************************************************************/


/*****************************************************************************
 * Handlers de interrupciones
*****************************************************************************/
/* Handler del ADC0 */
void ADC0_IRQHandler(void){
	Chip_ADC_ReadValue(LPC_ADC0, ADC_CH0,&adc_data);
}

/* Handler de las interrupciones del GPIO */
void GPIO0_IRQHandler(void){
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(PHASE_PININT_INDEX));	//Se limpian las interrupciones

	xSemaphoreGiveFromISR(xPhaseSemaphore, &xHigherPriorityTaskWoken);	//Se otorga el semaforo
	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);		//Se fuerza un cambio de contexto si es necesario
}

/* Handler del timer */
void TIMER1_IRQHandler(void){
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	Chip_TIMER_Disable(LPC_TIMER1);	//Se apaga el timer. El timer se debe activar solo cuando se lo requiere
	Chip_TIMER_ClearMatch(LPC_TIMER1, 1);	//Se limpian las interrupciones pendientes

	xSemaphoreGiveFromISR(xTimerSemaphore, &xHigherPriorityTaskWoken);	//Se otorga el semaforo del timer
	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);		//Se fuerza un cambio de contexto si es necesario
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
	ConfigureTimer();	//Se configura el timer para la generacion del delay del trigger
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
	LPC_GPIO_PIN_INT->SIENR |= PININTCH(PHASE_PININT_INDEX);						//Se setean los flancos ascendentes
	LPC_GPIO_PIN_INT->SIENF |= PININTCH(PHASE_PININT_INDEX);						//Se setean los flancos descendentes

	/* Habilitacion de las interrupciones */
	NVIC_SetPriority(PIN_INT0_IRQn, PHASE_INTERRUPT_PRIORITY);	//Se setea la prioridad de la interrupcion
	NVIC_ClearPendingIRQ(PIN_INT0_IRQn);	//Se limpian las interrupciones pendientes
	NVIC_EnableIRQ(PIN_INT0_IRQn);			//Se habilitan las interrupciones para el pin
}

/* Configuracion del timer */
void ConfigureTimer( void ){
	Chip_TIMER_Init(LPC_TIMER1);	//Se habilita el timer
	Chip_RGU_TriggerReset(RGU_TIMER1_RST);	//Se resetea el periferico del timer
	while (Chip_RGU_InReset(RGU_TIMER1_RST));

	Chip_TIMER_Reset(LPC_TIMER1);				//Se resetea el timer y se inicializa en cero
	Chip_TIMER_MatchEnableInt(LPC_TIMER1, 1);	//Se habilitan las interrupciones por match. Cuando el timer
												//alcanza el valor del registro, se dispara la interrupcion
	Chip_TIMER_SetMatch(LPC_TIMER1, 1,0);	//Se inicializa el valor maximo del timer
	Chip_TIMER_ResetOnMatchEnable(LPC_TIMER1, 1);	//Se resetea al timer automaticamente luego de alcanzar el maximo

	NVIC_SetPriority(TIMER1_IRQn, TIMER_INTERRUPT_PRIORITY);	//Se setea la prioridad de la interrupcion del timer

	NVIC_EnableIRQ(TIMER1_IRQn);				//Se habilitan las interrupciones del timer
	NVIC_ClearPendingIRQ(TIMER1_IRQn);			//Se limpian las interrupciones pendientes del timer

	/* Configuracion del pin de trigger del triac */
	Chip_SCU_PinMuxSet(TRIGGER_SCU_INT_PORT, TRIGGER_SCU_INT_PIN,(SCU_MODE_INBUFF_EN|SCU_MODE_INACT|SCU_MODE_FUNC0));
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, TRIGGER_GPIO_INT_PORT, TRIGGER_GPIO_INT_PIN);	//Pin de salida
	Chip_GPIO_SetPinState(LPC_GPIO_PORT, TRIGGER_GPIO_INT_PORT, TRIGGER_GPIO_INT_PIN, (bool) false);

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

        Chip_TIMER_SetMatch(LPC_TIMER1, 1,TIMER_BASE+TIMER_STEP*adc_data);	//Se configura el valor maximo del timer
        Chip_TIMER_Enable(LPC_TIMER1);			//Se habilita el timer para disparar el trigger

        /* To get here the event must have occurred.  Process the event (in this
         * case we just print out a message). */
        DEBUGOUT("Deteccion de cruce por cero.\r\n");
    }
}

/* Esta tarea se ejecuta siempre que se deba disparar el trigger, es
 * decir que el timer finalizo la cuenta.*/
static void vHandlerFireTrigger(void *pvParameters){
	portTickType xLastExecutionTime;

	/* Se toma el semaforo para vaciarlo antes de entrar al loop infinito */
    xSemaphoreTake(xTimerSemaphore, (portTickType) 0);

	while (1) {

		/* La tarea permanece bloqueada hasta que el semaforo se libera */
        xSemaphoreTake(xTimerSemaphore, portMAX_DELAY);

        /* Se evita que el scheduler retire la CPU antes de disparar el trigger */
        taskENTER_CRITICAL();
        DEBUGOUT("Se dispara el trigger.\r\n");
        	/* Se lee el instante inicial para tener un timer preciso */
			xLastExecutionTime = xTaskGetTickCount();

			/* Se dispara el trigger */
			Chip_GPIO_SetPinState(LPC_GPIO_PORT, TRIGGER_GPIO_INT_PORT, TRIGGER_GPIO_INT_PIN, (bool) true);
				DEBUGOUT("El delay es: %i\n\r", (adc_data*TIMER_STEP+TIMER_BASE)/ portTICK_RATE_MS);
				/* Se espera cierto tiempo para asegurar que el triac pueda ser encendido */
				vTaskDelayUntil(&xLastExecutionTime, 1 / portTICK_RATE_MS);

			/* Se apaga el trigger */
			Chip_GPIO_SetPinState(LPC_GPIO_PORT, TRIGGER_GPIO_INT_PORT, TRIGGER_GPIO_INT_PIN, (bool) false);
		DEBUGOUT("Se apaga el trigger.\r\n");
        /* Una vez disparado el trigger se libera la CPU */
        taskEXIT_CRITICAL();
    }
}
/*****************************************************************************/


/*****************************************************************************
 * Funcion main
 ****************************************************************************/
int main(void){
	SetupHardware();	//Se inicializa el hardware

	vSemaphoreCreateBinary(xPhaseSemaphore);	//Se crea el semaforo para la deteccion de cruces por cero
	vSemaphoreCreateBinary(xTimerSemaphore);	//Se crea el semaforo para la activacion del timer

	/* Se verifica que el semaforo haya sido creado correctamente */
	if( (xPhaseSemaphore!=(xSemaphoreHandle)NULL)&&(xTimerSemaphore!=(xSemaphoreHandle)NULL) ){

		/* Se crea la tarea que se encarga de detectar los cruces por cero */
		xTaskCreate(vHandlerZeroCrossing, (char *) "ZeroCrossing", configMINIMAL_STACK_SIZE,
							(void *) 0, (tskIDLE_PRIORITY + 1UL), (xTaskHandle *) NULL);
		/* Se crea la tarea que se encarga de generar los disparos del trigger */
		xTaskCreate(vHandlerFireTrigger, (char *) "FireTrigger", configMINIMAL_STACK_SIZE,
							(void *) 0, (tskIDLE_PRIORITY + 2UL), (xTaskHandle *) NULL);


		vTaskStartScheduler(); /* Se comienzan a ejecutar las tareas. */
	}

	while (1);	//No se deberia llegar nunca a este punto.
return ((int) NULL);
}
#endif