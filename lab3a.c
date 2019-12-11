/*****************************************************************************
* lab2a.c for Lab2A of ECE 153a at UCSB
* Date of the Last Update:  October 23,2014
*****************************************************************************/

#define AO_LAB2A

#include "qpn_port.h"
#include "bsp.h"
#include "lab3a.h"
#include "gpio.h"
#include "lcd.h"
#include "timer.h"
#include "PmodNAV.h"
#include "step_counter.h"
//#include "math.h"



typedef struct Lab2ATag  {               //Lab2A State machine
	QActive super;
}  Lab2A;

/* Setup state machines */
/**********************************************************************/
static QState Lab3A_initial (Lab2A *me);
static QState Lab3A_on      (Lab2A *me);

static QState Calibrate_State  (Lab2A *me);
static QState Measure_State (Lab2A *me);

/**********************************************************************/


Lab2A AO_Lab3A;

void Lab3A_ctor(void)  {
	Lab2A *me = &AO_Lab3A;
	QActive_ctor(&me->super, (QStateHandler)&Lab3A_initial);
}


QState Lab3A_initial(Lab2A *me) {
	xil_printf("\n\rInitialization");

	/* set default values */
    major_axis = Z_AXIS;
    acc_thresh_z = 0.95;
    last_step = -1;
    stride = 0.512;

    return Q_TRAN(&Lab3A_on);
}

QState Lab3A_on(Lab2A *me) {
	switch (Q_SIG(me)) {
		case Q_ENTRY_SIG: {
			xil_printf("\n\rOn");
			}
			
		case Q_INIT_SIG: {
			xil_printf("lab 3A init sig\r\n");

			/* init nav + set odr + clear display */
			NAV_Init(&nav);
			clrScr();
			NAV_SetODR(&nav, NAV_ACL_GYRO_MODE_INST_AG, NAV_ACL_ODR_XL_119HZ);

			//set offsets for mag registers
//			NAV_WriteSPI(&nav, NAV_INST_MAG, NAV_MAG_OFFSET_X_REG_H_M, 0x0); //0x00E4
//			NAV_WriteSPI(&nav, NAV_INST_MAG, NAV_MAG_OFFSET_X_REG_L_M, 0xF7);
//			NAV_WriteSPI(&nav, NAV_INST_MAG, NAV_MAG_OFFSET_Y_REG_H_M, 0x0); //0x0090
//			NAV_WriteSPI(&nav, NAV_INST_MAG, NAV_MAG_OFFSET_Y_REG_L_M, 0x9B);
//			NAV_WriteSPI(&nav, NAV_INST_MAG, NAV_MAG_OFFSET_Z_REG_H_M, 0xFE); //0xFEEA
//			NAV_WriteSPI(&nav, NAV_INST_MAG, NAV_MAG_OFFSET_Z_REG_L_M, 0xF1);

			local_min_acc_z = 100;
			local_max_acc_z = -100;
			cal_set = 0;
			return Q_TRAN(&Calibrate_State);
			}
	}
	
	return Q_SUPER(&QHsm_top);
}


/* Create Lab2A_on state and do any initialization code if needed */
/******************************************************************/
//output: acc major axis, initial threshold
QState Calibrate_State  (Lab2A *me){
	switch (Q_SIG(me)) {
			case Q_ENTRY_SIG: {

				xil_printf("entered the calibration\r\n");
				XGpio_DiscreteWrite(&Gpio_RGB_LED, LED_CHANNEL, LD16_GREEN);

				/* reset variables */
				timerTrigger = 0;
				steps = 0;
				state = CAL_BTN;

				/* display default text + steps */
				fillRect(0, 70, DISP_X_SIZE, DISP_Y_SIZE);
				display_cal_directions( lcd_buffer, BUF_SIZE, CAL_BTN);

				return Q_HANDLED();
			}
			case Q_EXIT_SIG: {
				//TODO: get origin elevation
				XTmrCtr_Stop(&axiTimer, 0);

				/* get origin elevation (m)				 */
				NAV_ReadPressurehPa(&nav);
				RefElevation = Nav_GetOrigin_Alt( nav.hPa , 20 ); //20 ft refrence elevation

				/* determine initial values for measure */
				major_axis = Z_AXIS;
				stride = determine_stride(steps);
				sample_num = 0;

				return Q_HANDLED();
			}
			case CALIBRATE_SIG: {
				return Q_HANDLED();
			}
			case MEASURE_SIG: {
				return Q_TRAN(&Measure_State);
			}
			case TICK_SIG:{
				//TODO: add the
				/* calc new threshold */
				calibrate_acc_threshold();
				sample_num = 0;

				/* timeout if a minute went by */
				if( timerTrigger > 60 * 12) QActive_postISR((QActive *)&AO_Lab3A, STOP_SIG);
				return Q_HANDLED();
			}
			case START_SIG: {
				/* display directions */
				fillRect(0, 70, DISP_X_SIZE, 150);
//				XTmrCtr_Start(&axiTimer, 0);
				display_cal_directions( lcd_buffer, BUF_SIZE, START_BTN);

				XTmrCtr_Start(&axiTimer, 0);

				return Q_HANDLED();
			}
			case STOP_SIG:{
				XTmrCtr_Stop(&axiTimer, 0);
				//TODO: should set this location as origin point?
				/* display directions */
				fillRect(0, 70, DISP_X_SIZE, 150);
				display_cal_directions( lcd_buffer, BUF_SIZE, STOP_BTN);
				return Q_HANDLED();
			}
			case WAYPOINT_SIG:{
				//do nothing - in calibrate
				return Q_HANDLED();
			}

	}
	return Q_SUPER(&Lab3A_on);

}

//output to display: number steps taken, distance covered by last step, direction of last step
QState Measure_State  (Lab2A *me){
	switch (Q_SIG(me)) {
			case Q_ENTRY_SIG: {
				int x = 0;
				xil_printf("entered the measurement\r\n");
				XGpio_DiscreteWrite(&Gpio_RGB_LED, LED_CHANNEL, LD16_BLUE);

				/* reset variables */
				for ( x = 0; x < 100; x++) th[x] = 0;
				magCount = 0;
				timerTrigger = 0;
				sample_num = 0;
				steps = 0;
				state = MEASURE_BTN;

				/* display directions + steps */
				display_meas_directions( lcd_buffer, BUF_SIZE, MEASURE_BTN);
				display_steps(lcd_buffer, BUF_SIZE);

				XTmrCtr_Start(&axiTimer, 0);
				return Q_HANDLED();
			}
			case Q_EXIT_SIG: {
				/* stop timer before exit */
				XTmrCtr_Stop(&axiTimer, 0);
				sample_num = 0;

				//TODO: need to reset all of the waypiont shit

				return Q_HANDLED();
			}
			case CALIBRATE_SIG: {
				return Q_TRAN(&Calibrate_State);
			}
			case MEASURE_SIG: {
				return Q_HANDLED();
			}
			case TICK_SIG: {
				/* calculate new threshold*/
				calibrate_acc_threshold();
				sample_num = 0;
				return Q_HANDLED();
			}
			case START_SIG: {

				return Q_HANDLED();
			}
			//TODO: make this cycle through all of the way points
			case STOP_SIG: {
				/* display text */
				fillRect(0, 70, DISP_X_SIZE, 220);
				display_meas_directions( lcd_buffer, BUF_SIZE, STOP_BTN);
				return Q_HANDLED();
			}
			case WAYPOINT_SIG:{
				//stop timer


				XTmrCtr_Stop(&axiTimer, 0);
				fillRect(0, 70, DISP_X_SIZE, 130);
				int s = 0;

				//calculate distance and direction from origin
				local_dx = 0;
				local_dy = 0;

				for( s = 0; s < 100; s++){
					if( th[s] == 0) break;
					local_dx += find_cosine( th[s] );
					local_dy += find_sine( th[s] );
				}

				//covert to distance in meters
				local_dx = local_dx * stride;
				local_dy = local_dy * stride;

				//now add dx + dx(previous waypoints)
				for( s = 0; s<8; s++){
					if( s == waypoint) break;
					dx[waypoint] += dx[s];
					dy[waypoint] += dy[s];
				}

				dx[waypoint] += local_dx;
				dy[waypoint] += local_dy;

				dist[waypoint] = find_dist( dx[waypoint], dy[waypoint] );
				//TODO: make sure value is between -1 and 1 before doing an approximation
//				if ( dx[waypoint ] > dy[waypoint] ) X = ( 1 / (dy[waypoint] / dx[waypoint] ));
				dir[waypoint] = find_dir( dx[waypoint], dy[waypoint] ); //from origin to waypoint

				//display results
				display_waypoint( lcd_buffer, BUF_SIZE, waypoint, dist[waypoint], dir[waypoint]);
				//restart timer + reset magCount
				for( s = 0; s < 100; s++) th[s] = 0; //clear theta array
				magCount = 0;
				waypoint++;

				XTmrCtr_Start(&axiTimer, 0);

				return Q_HANDLED();
			}


	}
	return Q_SUPER(&Lab3A_on);

}

