/*
 * gpio.c
 *
 *  Created on: Oct 21, 2019
 *      Author: blake
 */
#include "gpio.h"
#include "xparameters.h"
#include "xgpio.h"
#include "xtmrctr_l.h"
#include "lcd.h"
#include "bsp.h"
#include "qpn_port.h"
#include "lab3a.h"

void init_gpio_lab2a(){

	   XGpio_Initialize(&Gpio_LED, XPAR_GPIO_2_DEVICE_ID); //LEDs
	   XGpio_SetDataDirection(&Gpio_LED, LED_CHANNEL, 0); //outputs

	   XGpio_Initialize(&Gpio_BTN, XPAR_GPIO_1_DEVICE_ID); //Push Buttons
	   XGpio_SetDataDirection(&Gpio_BTN, LED_CHANNEL, 0xffff); //inputs

	   XGpio_Initialize(&Gpio_RGB_LED, XPAR_GPIO_3_DEVICE_ID); //RGB LEDs
	   XGpio_SetDataDirection(&Gpio_RGB_LED, LED_CHANNEL, 0); //outputs

	   XGpio_Initialize(&dc, XPAR_SPI_DC_DEVICE_ID);	//DC signal for Display
	   XGpio_SetDataDirection(&dc, 1, 0x0);

}

void init_timer_button(){
	//TIMER RESET CODE
	//Turn off the timer
	XTmrCtr_SetControlStatusReg(XPAR_TMRCTR_0_BASEADDR, 1, 0);

	//Put a zero in the load register
	XTmrCtr_SetLoadReg(XPAR_TMRCTR_0_BASEADDR, 1, 0);

	//Copy the load register into the counter register
	XTmrCtr_SetControlStatusReg(XPAR_TMRCTR_0_BASEADDR, 1, XTC_CSR_LOAD_MASK);

	//Enable (start) the timer
	XTmrCtr_SetControlStatusReg(XPAR_TMRCTR_0_BASEADDR, 1,	XTC_CSR_ENABLE_TMR_MASK);
}

void setup_interrupt_btns(){
	Status = XST_SUCCESS;
		Status = XIntc_Connect(&sys_intc,XPAR_MICROBLAZE_0_AXI_INTC_AXI_GPIO_BTN_IP2INTC_IRPT_INTR,
				(XInterruptHandler)button_handler, &Gpio_BTN);
		if ( Status != XST_SUCCESS )
		{
			xil_printf("Failed to connect the application handlers to the interrupt controller (button)...\r\n");
			return;
		}
		xil_printf("Connected Buttons to Interrupt Controller!\r\n");

		XGpio_InterruptEnable( &Gpio_BTN, 0xff );
		XGpio_InterruptGlobalEnable( &Gpio_BTN );

		XIntc_Enable(&sys_intc, XPAR_MICROBLAZE_0_AXI_INTC_AXI_GPIO_BTN_IP2INTC_IRPT_INTR);
			Status = XIntc_SelfTest (&sys_intc);
			if ( Status != XST_SUCCESS )
			{
				xil_printf("self test failed...\r\n");
				return;
			}
			xil_printf("self test passed (button)!\r\n");

			Status = XIntc_Start(&sys_intc, XIN_REAL_MODE);
			if ( Status != XST_SUCCESS )
			{
				xil_printf("Interrupt controller driver failed to start...\r\n");
				return;
			}
			xil_printf("Started Interrupt Controller (button)!\r\n");
}

//TODO: need some form of debouncing
void button_handler(void *CallbackRef){
	XGpio *GpioPtr = (XGpio *)CallbackRef;

	btnValue = XGpio_DiscreteRead(&Gpio_BTN, 1);
	if( btnValue != 0){
		lastbtnValue = btnValue;
		switch( lastbtnValue ){
		case 1:
			//dispatch start
			QActive_postISR((QActive *)&AO_Lab3A, START_SIG);
			break;
		case 2:
			//dispatch calibrate
			QActive_postISR((QActive *)&AO_Lab3A, CALIBRATE_SIG);
			break;
		case 4:
			//dispatch measure
			QActive_postISR((QActive *)&AO_Lab3A, MEASURE_SIG);
			break;
		case 8:
			//dispatch stop
			QActive_postISR((QActive *)&AO_Lab3A, STOP_SIG);
			break;
		case 16:
			break;
		default: break;
		}
	}

	/* Clear the Interrupt */
		XGpio_InterruptClear(GpioPtr, 1);
}

