/*
 * timer.h
 *
 *  Created on: Oct 29, 2019
 *      Author: blake
 */

#ifndef SRC_TIMER_H_
#define SRC_TIMER_H_

#include "xtmrctr.h"
#include "xintc.h"

XTmrCtr axiTimer;

volatile int timerTrigger; //when 10 the display turns off
volatile int i;

#define RESET_VALUE     1000000  //0.01 sec

int led_toggle;


void init_timer_interrupt();
void TimerCounterHandler(void *CallBackRef, u8 TmrCtrNumber);


#endif /* SRC_TIMER_H_ */
