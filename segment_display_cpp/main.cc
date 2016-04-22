/*
 * Empty C++ Application
 */

#include "xparameters.h"
#include "xgpio.h"
#include "xil_printf.h"

#define DISPLAY_DEVICE_ID	XPAR_AXI_GPIO_0_DEVICE_ID
#define SWITCHES_DEVICE_ID	XPAR_AXI_GPIO_0_DEVICE_ID
#define CHANNEL_ONE			1
#define CHANNEL_TWO			2
#define DIRECTION_OUT		0x0
#define DIRECTION_IN		0xFFFFFFFF

#define DIGIT_ONE			0b00000000
#define DIGIT_TWO			0b10000000
#define SS_ZERO				0b00111111
#define SS_ONE				0b00000110
#define SS_TWO				0b01011011
#define SS_THREE			0b01001111
#define SS_FOUR				0b01100110
#define SS_FIVE				0b01101101
#define SS_SIX				0b01111101
#define SS_SEVEN			0b00000111
#define SS_EIGHT			0b01111111
#define SS_NINE				0b01101111

#define SS_A				0b01110111
#define SS_B				0b01111111
#define SS_C				0b00111001
#define SS_D				0b00111111
#define SS_E				0b01111001
#define SS_F				0b01110001

static XGpio gpio_digits;
static XGpio gpio_switches;

long initDisplay() {
	int xStatus = XGpio_Initialize(&gpio_digits, DISPLAY_DEVICE_ID);
	if (XST_SUCCESS != xStatus) {
		print("Digits init failed\n\r");
		return XST_FAILURE;
	}
	XGpio_SetDataDirection(&gpio_digits, CHANNEL_ONE, DIRECTION_OUT);
	return XST_SUCCESS;
}

long initSwitches() {
	int xStatus = XGpio_Initialize(&gpio_switches, SWITCHES_DEVICE_ID);
	if (XST_SUCCESS != xStatus) {
		print("Switches init failed\n\r");
		return XST_FAILURE;
	}
	XGpio_SetDataDirection(&gpio_switches, CHANNEL_TWO, DIRECTION_IN);
	return XST_SUCCESS;
}

void fixedDisplay() {
	XGpio_DiscreteWrite(&gpio_digits, CHANNEL_ONE, DIGIT_ONE bitor SS_FOUR);
}

void fixedDisplayLoop() {
	XGpio_DiscreteWrite(&gpio_digits, CHANNEL_ONE, DIGIT_ONE bitor SS_TWO);
	XGpio_DiscreteWrite(&gpio_digits, CHANNEL_ONE, DIGIT_TWO bitor SS_FOUR);
}

void writeIntToDigit(int number, u32 digit) {
	switch (number) {
	case 15:
		XGpio_DiscreteWrite(&gpio_digits, CHANNEL_ONE, digit bitor SS_F);
		break;
	case 14:
		XGpio_DiscreteWrite(&gpio_digits, CHANNEL_ONE, digit bitor SS_E);
		break;
	case 13:
		XGpio_DiscreteWrite(&gpio_digits, CHANNEL_ONE, digit bitor SS_D);
		break;
	case 12:
		XGpio_DiscreteWrite(&gpio_digits, CHANNEL_ONE, digit bitor SS_C);
		break;
	case 11:
		XGpio_DiscreteWrite(&gpio_digits, CHANNEL_ONE, digit bitor SS_B);
		break;
	case 10:
		XGpio_DiscreteWrite(&gpio_digits, CHANNEL_ONE, digit bitor SS_A);
		break;
	case 9:
		XGpio_DiscreteWrite(&gpio_digits, CHANNEL_ONE, digit bitor SS_NINE);
		break;
	case 8:
		XGpio_DiscreteWrite(&gpio_digits, CHANNEL_ONE, digit bitor SS_EIGHT);
		break;
	case 7:
		XGpio_DiscreteWrite(&gpio_digits, CHANNEL_ONE, digit bitor SS_SEVEN);
		break;
	case 6:
		XGpio_DiscreteWrite(&gpio_digits, CHANNEL_ONE, digit bitor SS_SIX);
		break;
	case 5:
		XGpio_DiscreteWrite(&gpio_digits, CHANNEL_ONE, digit bitor SS_FIVE);
		break;
	case 4:
		XGpio_DiscreteWrite(&gpio_digits, CHANNEL_ONE, digit bitor SS_FOUR);
		break;
	case 3:
		XGpio_DiscreteWrite(&gpio_digits, CHANNEL_ONE, digit bitor SS_THREE);
		break;
	case 2:
		XGpio_DiscreteWrite(&gpio_digits, CHANNEL_ONE, digit bitor SS_TWO);
		break;
	case 1:
		XGpio_DiscreteWrite(&gpio_digits, CHANNEL_ONE, digit bitor SS_ONE);
		break;
	case 0:
		XGpio_DiscreteWrite(&gpio_digits, CHANNEL_ONE, digit bitor SS_ZERO);
		break;
	}
}

void fixedLoop(int number) {
	int digit_two = 0;
	if (number > 15) {
		digit_two = number / 16;
	}
	int digit_one = (number - (digit_two * 16));

	writeIntToDigit(digit_two, DIGIT_TWO);
	writeIntToDigit(digit_one, DIGIT_ONE);
}

void displaySwitches() {
	int digit_one = 0;
	int digit_two = 0;

	int switches = XGpio_DiscreteRead(&gpio_switches, CHANNEL_TWO);
	if (switches >= 128) {
		digit_two += 8;
		switches -= 128;
	}
	if (switches >= 64) {
		digit_two += 4;
		switches -= 64;
	}
	if (switches >= 32) {
		digit_two += 2;
		switches -= 32;
	}
	if (switches >= 16) {
		digit_two += 1;
		switches -= 16;
	}
	if (switches >= 8) {
		digit_one += 8;
		switches -= 8;
	}
	if (switches >= 4) {
		digit_one += 4;
		switches -= 4;
	}
	if (switches >= 2) {
		digit_one += 2;
		switches -= 2;
	}
	if (switches >= 1) {
		digit_one += 1;
		switches -= 1;
	}

	writeIntToDigit(digit_two, DIGIT_TWO);
	writeIntToDigit(digit_one, DIGIT_ONE);
}

int main() {
	initDisplay();
	initSwitches();

	print("Segment Display\n");

	while (true) {
		displaySwitches();
	}

	return 0;
}
