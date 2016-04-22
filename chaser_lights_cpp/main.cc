#include "xparameters.h"
#include "xgpio.h"
#include "xgpiops.h"
#include "xil_printf.h"
#include <stdio.h>
#include "xil_io.h"
#include "xil_types.h"
#include "xil_io.h"
#include "xil_exception.h"
#include "xscugic.h"
#include "xscutimer.h"

#define GPIO_INTERRUPT_ID XPAR_FABRIC_AXI_GPIO_1_IP2INTC_IRPT_INTR
#define TIMER_INTERRUPT_ID XPAR_SCUTIMER_INTR
#define LEDS_DEVICE_ID XPAR_AXI_GPIO_0_DEVICE_ID
#define SWITCHES_DEVICE_ID XPAR_AXI_GPIO_0_DEVICE_ID
#define BUTTONS_DEVICE_ID XPAR_AXI_GPIO_1_DEVICE_ID
#define INTC_DEVICE_ID  XPAR_SCUGIC_SINGLE_DEVICE_ID
#define TIMER_DEVICE_ID XPAR_SCUTIMER_DEVICE_ID

#define CHANNEL_ONE	1
#define CHANNEL_TWO	2
#define DIRECTION_OUT 0x0
#define DIRECTION_IN 0xFFFFFFFF

#define TIMER_LOAD_VALUE 333000000

#define LED_0 1
#define LED_1 2
#define LED_2 4
#define LED_3 8
#define LED_4 16
#define LED_5 32
#define LED_6 64
#define LED_7 128

#define BTNC 1
#define BTND 2
#define BTNL 4
#define BTNR 8
#define BTNU 16

#define SW_0 1
#define SW_1 2
#define SW_2 4
#define SW_3 8
#define SW_4 16
#define SW_5 32
#define SW_6 64
#define SW_7 128

static XGpio GpioLeds;
static XGpio GpioButtons;
static XGpio GpioSwitches;

XScuTimer timer;
static XScuTimer_Config *TMRConfigPtr;

XScuGic InterruptController; /* Instance of the Interrupt Controller */
static XScuGic_Config *GicConfig;/* The configuration parameters of the controller */

int LEDS[] = { LED_0, LED_1, LED_2, LED_3, LED_4, LED_5, LED_6, LED_7 };

bool leds_dir_left = 0;
bool leds_running = 1;
int led_speed = 1;
int ledsData = 3;

void INTERRUPT_Handler0(void *baseaddr_p) {
	/* Handle interrupts from buttons */
	if (GpioButtons.InterruptPresent) {
		XGpio_InterruptClear(&GpioButtons, XGPIO_IR_CH1_MASK);

		switch (XGpio_DiscreteRead(&GpioButtons, CHANNEL_ONE)) {
		case BTNC:
			leds_running = !leds_running;
			break;
		case BTNU:
			break;
		case BTND:
			break;
		case BTNL:
			leds_dir_left = 1;
			break;
		case BTNR:
			leds_dir_left = 0;
			break;
		}
	}
}

long initLEDs() {
	int xStatus = XGpio_Initialize(&GpioLeds, LEDS_DEVICE_ID);
	if (XST_SUCCESS != xStatus) {
		print("GPIO LED INIT FAILED\n\r");
		return XST_FAILURE;
	}
	XGpio_SetDataDirection(&GpioLeds, CHANNEL_ONE, DIRECTION_OUT);
	return XST_SUCCESS;
}

long initSwitches() {
	int xStatus = XGpio_Initialize(&GpioSwitches, SWITCHES_DEVICE_ID);
	if (XST_SUCCESS != xStatus) {
		print("GPIO SWITCH INIT FAILED\n\r");
		return XST_FAILURE;
	}
	XGpio_SetDataDirection(&GpioSwitches, CHANNEL_TWO, DIRECTION_IN);
	return XST_SUCCESS;
}

long initButtons() {
	int xStatus = XGpio_Initialize(&GpioButtons, BUTTONS_DEVICE_ID);
	if (XST_SUCCESS != xStatus) {
		print("GPIO BUTTON INIT FAILED\n\r");
		return XST_FAILURE;
	}
	XGpio_SetDataDirection(&GpioButtons, CHANNEL_ONE, DIRECTION_IN);
	return XST_SUCCESS;
}

void writeLED(int ledsData) {
	XGpio_DiscreteWrite(&GpioLeds, CHANNEL_ONE, LEDS[ledsData]);
}

void setLeds() {
	if (leds_running) {
		if (ledsData == 0)
			leds_dir_left = 1;
		else if (ledsData == 7)
			leds_dir_left = 0;
		if (leds_dir_left)
			ledsData = (ledsData + 1) % 8;
		else
			ledsData = (ledsData - 1) % 8;
		writeLED(ledsData);
	}
}

void checkSwitches() {
	int newSpeed = 0;
	int read = XGpio_DiscreteRead(&GpioSwitches, CHANNEL_TWO);
	while (read > 0) {
		if (read >= SW_7) {
			newSpeed += 1;
			read -= SW_7;
		}
		if (read >= SW_6) {
			newSpeed += 2;
			read -= SW_6;
		}
		if (read >= SW_5) {
			newSpeed += 3;
			read -= SW_5;
		}
		if (read >= SW_4) {
			newSpeed += 4;
			read -= SW_4;
		}
		if (read >= SW_3) {
			newSpeed += 5;
			read -= SW_3;
		}
		if (read >= SW_2) {
			newSpeed += 6;
			read -= SW_2;
		}
		if (read >= SW_1) {
			newSpeed += 7;
			read -= SW_1;
		}
		if (read >= SW_0) {
			newSpeed += 8;
			read -= SW_0;
		}
	}
	led_speed = newSpeed;
}

void TimerIntrHandler(void *CallBackRef) {
	XScuTimer_ClearInterruptStatus(&timer);
	checkSwitches();
	if (led_speed != 0) {
		/* Eine Sekunde für die ganze Strecke */
		//int timerValue = (TIMER_LOAD_VALUE / 8) * led_speed;
		/* Schnellere Variante */
		int timerValue = (TIMER_LOAD_VALUE) * led_speed;
		XScuTimer_LoadTimer(&timer, timerValue);
		XScuTimer_Start(&timer);
		setLeds();
	} else {
		int timerValue = (TIMER_LOAD_VALUE / 32);
		XScuTimer_LoadTimer(&timer, timerValue);
		XScuTimer_Start(&timer);
	}
}

int initTimer() {
	//timer initialisation
	TMRConfigPtr = XScuTimer_LookupConfig(TIMER_DEVICE_ID);
	XScuTimer_CfgInitialize(&timer, TMRConfigPtr, TMRConfigPtr->BaseAddr);
	//load the timer
	XScuTimer_LoadTimer(&timer, TIMER_LOAD_VALUE);

	//set up the timer interrupt
	int Status = XScuGic_Connect(&InterruptController, TIMER_INTERRUPT_ID,
			(Xil_ExceptionHandler) TimerIntrHandler,
			(void *) &InterruptController);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	//enable the interrupt for the Timer at GIC
	XScuGic_Enable(&InterruptController, TIMER_INTERRUPT_ID);
	//enable interrupt on the timer
	XScuTimer_EnableInterrupt(&timer);
	return 0;
}

int ScuGicInterrupt_Init() {
	int Status;

	Xil_ExceptionInit();

	GicConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
	if (NULL == GicConfig) {
		print("CLOCK CONFIG FAILED\n\r");
		return XST_FAILURE;
	}

	Status = XScuGic_CfgInitialize(&InterruptController, GicConfig,
			GicConfig->CpuBaseAddress);
	if (Status != XST_SUCCESS) {
		print("CLOCK INIT FAILED\n\r");
		return XST_FAILURE;
	}

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT,
			(Xil_ExceptionHandler) XScuGic_InterruptHandler,
			(void *) &InterruptController);

	Status = XScuGic_Connect(&InterruptController, GPIO_INTERRUPT_ID,
			(Xil_ExceptionHandler) INTERRUPT_Handler0,
			(void *) &InterruptController);

	XScuGic_Enable(&InterruptController, GPIO_INTERRUPT_ID);

	Xil_ExceptionEnable();

	XScuGic_SetPriorityTriggerType(&InterruptController, GPIO_INTERRUPT_ID,
			0xa0, 3);

	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	return 0;
}

void initInterupts() {
	ScuGicInterrupt_Init();
}

int main() {
	initLEDs();
	initSwitches();
	initButtons();

	XGpio_InterruptEnable(&GpioButtons, XGPIO_IR_CH1_MASK);
	XGpio_InterruptGlobalEnable(&GpioButtons);

	initInterupts();

	initTimer();

	XScuTimer_LoadTimer(&timer, TIMER_LOAD_VALUE);
	//start timer
	XScuTimer_Start(&timer);

	print("Chaser Lights\n");

	ledsData = 7;
	setLeds();

	while (true) {
	}

	return 0;
}
