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
	STOP_SIG,
	WAYPOINT_SIG
};

int sample_num;
int state;

volatile float dist[8];
volatile float dir[8];
volatile float dx[8];
volatile float dy[8];
volatile float th[100]; //up to 100 steps between waypoints
volatile u8 waypoint;
volatile float local_dx;
volatile float local_dy;
volatile float RefElevation; //in meters


NAV_RectCoord Magnetometer_Data;

extern struct Lab2ATag AO_Lab3A;


void Lab3A_ctor(void);

#endif /* SRC_LAB3A_H_ */
