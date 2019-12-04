/*****************************************************************************
* bsp.c for Lab2A of ECE 153a at UCSB
* Date of the Last Update:  October 27,2019
*****************************************************************************/

/**/
#include "qpn_port.h"
#include "bsp.h"
#include "xintc.h"
#include "xil_exception.h"
#include "gpio.h"
#include "timer.h"
#include "spi.h"
#include "lcd.h"
#include "PmodNAV.h"
#include "step_counter.h"
/*****************************/

/* Define all variables and Gpio objects here  */

#define GPIO_CHANNEL1 1



/*..........................................................................*/
void BSP_init(void) {
	/* Setup interrupts and reference to interrupt handler function(s)  */
	init_gpio_lab2a();
	init_timer_interrupt();
	setup_interrupt_btns();

	/* init SPI + LCD Screen */
	init_spi();
	initLCD();

	/* init nav device */
	xil_printf("initialized nav\n");
	Nav_EnableCaches();
	NAV_begin ( // intialize the PmodNAV driver device
			&nav,
			XPAR_GPIO_NAV_BASEADDR,
			XPAR_SPI_NAV_BASEADDR
	);
}
/*..........................................................................*/
//happens after initial state is entered
void QF_onStartup(void) {                 /* entered with interrupts locked */

	/* Enable interrupts */
//	xil_printf("\n\rQF_onStartup\n"); // Comment out once you are in your complete program

}


void QF_onIdle(void) {        /* entered with interrupts locked */

    QF_INT_UNLOCK();                       /* unlock interrupts */
}

/* Do not touch Q_onAssert */
/*..........................................................................*/
void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
    (void)file;                                   /* avoid compiler warning */
    (void)line;                                   /* avoid compiler warning */
    QF_INT_LOCK();
    for (;;) {
    }
}

/* Interrupt handler functions here.  Do not forget to include them in lab2a.h!
To post an event from an ISR, use this template:
QActive_postISR((QActive *)&AO_Lab2A, SIGNALHERE);
Where the Signals are defined in lab2a.h  */

