/*
 * gpio.h
 *
 *  Created on: Oct 21, 2019
 *      Author: blake
 */

#ifndef SRC_GPIO_H_
#define SRC_GPIO_H_

#include "xparameters.h"
#include "xgpio.h"
#include "xintc.h"

//LED literals
#define LED_CHANNEL   1
#define LED_BLINK_DELAY 150000 * 2
#define LED_15		32768
#define LED_0		0x1
#define LED_OFF 	0x0

#define LD16_BLUE 	0x01
#define LD16_GREEN 	0x02
#define LD16_RED 	0x04
#define LD16_AQUA LD16_BLUE|LD16_GREEN
#define LD16_PURPLE LD16_BLUE|LD16_RED
#define LD16_WHITE  LD16_PURPLE|LD16_AQUA

#define LD17_BLUE 	0x08
#define LD17_GREEN 	0x10
#define LD17_RED 	0x20
#define LD17_AQUA LD17_BLUE|LD17_GREEN
#define LD17_PURPLE LD17_BLUE|LD17_RED
#define LD17_WHITE  LD17_PURPLE|LD17_AQUA

XGpio Gpio_BTN;
XGpio Gpio_LED;
XGpio Gpio_RGB_LED;
XGpio dc;

XStatus Status;

volatile unsigned short btnValue;
volatile unsigned short lastbtnValue;


volatile unsigned short led_position; //which LED is on...one hot
volatile unsigned short led_high;


void init_gpio_lab2a(void);
void init_timer_button(void);
void setup_interrupt_btns();
void button_handler(void *CallbackRef);


#endif /* SRC_GPIO_H_ */
