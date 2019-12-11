
#include "step_counter.h"
#include "stdlib.h"
#include "lab3a.h"
#include "PmodNAV.h"
#include "lcd.h"
#include "gpio.h"
//TODO: make this file only consider the z axis!
void calibrate_acc_threshold(void){
	/* set default values */
	int i = 0;

	local_min_acc_z = 100;
	local_max_acc_z = -100;
	last_step = -2;

	/* average sample i w/ 3 point average */
//	for ( int j = 2; j < 3; j++){
		for (i = (sample_num) - (SAMPLE_WINDOW-1); i < (sample_num) - 1; i++){
			accel_data[i] = (accel_data[i-1] + accel_data[i] + accel_data[i+1])/3.0;
		}
//	}

	/* update min and max*/
	for (i = (sample_num) - (SAMPLE_WINDOW-1); i < (sample_num); i++){
		if( accel_data[i] < local_min_acc_z) local_min_acc_z = accel_data[i];
		if( accel_data[i] > local_max_acc_z) local_max_acc_z = accel_data[i];
	}

	/* calculate new thresholds */
	if( local_max_acc_z - local_min_acc_z > 0.25){
		acc_thresh_z = (local_min_acc_z + local_max_acc_z)/(float)2.0;
	}

	cal_set = 1;

	XGpio_DiscreteWrite(&Gpio_RGB_LED, LED_CHANNEL, LD17_RED);

	/* determine new major axis */
	determine_major_axis();
}


int determine_acc_step(void){
//	int range;
	char *compass[8] = {"North", "North-East", "East", "South-East", "South",
			"South-West", "West", "North-West"};

	if(sample_num == 0) return steps;

	thresh = acc_thresh_z;
	/* if i-1 > thresh && i < thresh enter */
	if( accel_data[sample_num - 1] > thresh && accel_data[sample_num] <= thresh){

		//TODO: add a criteria that filters out based on no activity of Z axis
		if( local_max_acc_z - local_min_acc_z < 0.25 && cal_set ){
			if( accel_data[sample_num - 1] > local_max_acc_z || accel_data[sample_num - 1] < local_min_acc_z ){
				//move on
			}
			else if( accel_data[sample_num] > local_max_acc_z || accel_data[sample_num] < local_min_acc_z){
				//move on
			}
			else{
				return steps;
			}
		}

		/* discount step if within 0.25 seconds */
		if( !init_flag ){
			if( (sample_num - last_step < 10) && last_step != -2) return steps;
		}
		else{
			if( (sample_num - last_step < 5) && last_step != -2) return steps;
		}


//		if( last_step == -1 ) return steps;

		/* increment step + calculate direction */
		steps++;

		last_step = sample_num - 1;
		magXY = Nav_AngleInXY(Magnetometer_Data);
//		if( magXY > 270.0f) magXY -= 270;
//		else magXY += 90;

		//ternary
		magXY = ( magXY > 270.0f ) ? (magXY - 270) : (magXY + 90);

		//TODO: store magnetometer degrees
		th[magCount] = magXY;
		magCount++;

		str = compass[(int)((magXY + 22.5) / 45.0) % 8];
		display_steps(lcd_buffer, BUF_SIZE);
		return steps;

	}
	return steps;
}

void determine_major_axis(void){
	major_axis = Z_AXIS;
	++axis_count[major_axis];
}

//input: steps taken in 20 ft dist
//output: average stride (in meters)
float determine_stride(int localSteps){
	float meters = 6.096;
	return (float)(meters/localSteps);
}


