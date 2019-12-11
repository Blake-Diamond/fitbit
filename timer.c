/*
 * timer.c
 *
 *  Created on: Oct 29, 2019
 *      Author: blake
 */
#include "timer.h"
#include "xstatus.h"
#include "xparameters.h"
#include "gpio.h"
#include "lcd.h"
#include "bsp.h"
#include "qpn_port.h"
#include "lab3a.h"
#include "step_counter.h"
static u32 status;

void init_timer_interrupt(){
	/** init timer, interrupt controller */
		status = XTmrCtr_Initialize(&axiTimer, XPAR_AXI_TIMER_0_DEVICE_ID);
		if (status != XST_SUCCESS) {
			xil_printf("Initialize timer fail!\n");
			return;
		}

		status = XIntc_Initialize(&sys_intc, XPAR_INTC_0_DEVICE_ID);
		if (status != XST_SUCCESS) {
			xil_printf("Initialize interrupt controller fail!\n");
			return;
		}

		status = XIntc_Connect(&sys_intc,
					XPAR_MICROBLAZE_0_AXI_INTC_AXI_TIMER_0_INTERRUPT_INTR,
					(XInterruptHandler)XTmrCtr_InterruptHandler,
					(void *)&axiTimer);
		if (status != XST_SUCCESS) {
			xil_printf("Connect IHR fail!\n");
			return;
		}

		status = XIntc_Start(&sys_intc, XIN_REAL_MODE);
		if (status != XST_SUCCESS) {
			xil_printf("Start interrupt controller fail!\n");
			return;
		}

		// Enable interrupt
		XIntc_Enable(&sys_intc, XPAR_MICROBLAZE_0_AXI_INTC_AXI_TIMER_0_INTERRUPT_INTR);
		microblaze_enable_interrupts();

		/*
		 * Setup the handler for the timer counter that will be called from the
		 * interrupt context when the timer expires, specify a pointer to the
		 * timer counter driver instance as the callback reference so the handler
		 * is able to access the instance data
		 */
		XTmrCtr_SetHandler(&axiTimer, TimerCounterHandler, &axiTimer);

		/*
		 * Enable the interrupt of the timer counter so interrupts will occur
		 * and use auto reload mode such that the timer counter will reload
		 * itself automatically and continue repeatedly, without this option
		 * it would expire once only
		 */
		XTmrCtr_SetOptions(&axiTimer, 0,
					XTC_INT_MODE_OPTION | XTC_AUTO_RELOAD_OPTION);

		/*
		 * Set a reset value for the timer counter such that it will expire
		 * eariler than letting it roll over from 0, the reset value is loaded
		 * into the timer counter when it is started
		 */
		XTmrCtr_SetResetValue(&axiTimer, 0, 0xFFFFFFFF-RESET_VALUE);

		i = 0;
		timerTrigger = 0;
}

/* dispatches tick every second - based on timer 0  */
void TimerCounterHandler(void *CallBackRef, u8 TmrCtrNumber)
{
	timerTrigger++;

	/* read new data */
	NAV_GetData(&nav);
	accel_data[sample_num] = nav.acclData.Z;
	Magnetometer_Data = nav.magData;

	/*determine step */
	determine_acc_step();
	sample_num++;
	if( sample_num == 99 ) init_flag = 1;
	/* toggle LED */
	if (sample_num % 24 == 0 && sample_num != 0){
		XGpio_DiscreteWrite(&Gpio_LED, LED_CHANNEL, led_toggle);
		led_toggle ^= 1;
	}

	/* determine new thresholds */
	if( timerTrigger % SAMPLE_WINDOW == 0 && (timerTrigger != 0) ){
		QActive_postISR((QActive *)&AO_Lab3A, TICK_SIG); //dispatch a tick signal
	}
}

