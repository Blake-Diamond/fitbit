/*
 * lab3a.h
 *
 *  Created on: Nov 18, 2019
 *      Author: blake
 */

#ifndef SRC_LAB3A_H_
#define SRC_LAB3A_H_
#include "qepn.h"
#include "PmodNAV.h"
//TODO: define signals for lab 3
enum Lab3ASignals {
	CALIBRATE_SIG = Q_USER_SIG,
	MEASURE_SIG,
	TICK_SIG,
	START_SIG,
	STOP_SIG
};

int sample_num;
int state;

//float gyro_data[3][100];
//float degree_data[100];

NAV_RectCoord Magnetometer_Data;

extern struct Lab2ATag AO_Lab3A;


void Lab3A_ctor(void);

#endif /* SRC_LAB3A_H_ */
