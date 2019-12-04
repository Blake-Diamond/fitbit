
#include "step_counter.h"
#include "stdlib.h"
#include "lab3a.h"
#include "PmodNAV.h"
#include "lcd.h"
void calibrate_acc_threshold(void){
	/* set default values */
	int i = 0;
	local_min_acc_x = 100;
	local_min_acc_y = 100;
	local_min_acc_z = 100;
	local_max_acc_x = -100;
	local_max_acc_y = -100;
	local_max_acc_z = -100;
	last_step = -1;

	/* average sample i w/ 3 point average */
	for ( int j = 0; j < 3; j++){
		for (i = (sample_num) - (SAMPLE_WINDOW-1); i < (sample_num) - 1; i++){
			accel_data[j][i] = (accel_data[j][i-1] + accel_data[j][i] + accel_data[j][i+1])/3.0;
		}
	}

	/* update min and max*/
	for (i = (sample_num) - (SAMPLE_WINDOW-1); i < (sample_num); i++){
		if( accel_data[0][i] < local_min_acc_x) local_min_acc_x = accel_data[0][i];
		if( accel_data[1][i] < local_min_acc_y) local_min_acc_y = accel_data[1][i];
		if( accel_data[2][i] < local_min_acc_z) local_min_acc_z = accel_data[2][i];
		if( accel_data[0][i] > local_max_acc_x) local_max_acc_x = accel_data[0][i];
		if( accel_data[1][i] > local_max_acc_y) local_max_acc_y = accel_data[1][i];
		if( accel_data[2][i] > local_max_acc_z) local_max_acc_z = accel_data[2][i];
	}

	/* calculate new thresholds */
	acc_thresh_x = (local_min_acc_x + local_max_acc_x)/(float)2.0;
	acc_thresh_y = (local_min_acc_y + local_max_acc_y)/(float)2.0;
	acc_thresh_z = (local_min_acc_z + local_max_acc_z)/(float)2.0;

	/* determine new major axis */
	determine_major_axis();
}


int determine_acc_step(void){

	char *compass[8] = {"North", "North-East", "East", "South-East", "South",
			"South-West", "West", "North-West"};

	if(sample_num == 0) return steps;

	switch( major_axis ){
	case X_AXIS:
		thresh = acc_thresh_x;
		break;
	case Y_AXIS:
		thresh = acc_thresh_y;
		break;
	case Z_AXIS:
		thresh = acc_thresh_z;
		break;
	}

	/* if i-1 > thresh && i < thresh enter */
	if( accel_data[major_axis][sample_num - 1] > thresh && accel_data[major_axis][sample_num] <= thresh){

		/* discount step if within 0.25 seconds */
		if( (sample_num - last_step < 5) && last_step != -1) return steps;

		/* increment step + calculate direction */
		steps++;
		last_step = sample_num - 1;
		magXY = Nav_AngleInXY(Magnetometer_Data);
		str = compass[(int)((magXY + 22.5) / 45.0) % 8];
		display_steps(lcd_buffer, BUF_SIZE);
		return steps;

	}
	return steps;
}

void determine_major_axis(void){
	int x_diff = abs(local_max_acc_x - local_min_acc_x);
	int y_diff = abs(local_max_acc_y - local_min_acc_y);
	int z_diff = abs(local_max_acc_z - local_min_acc_z);

	if ( x_diff > y_diff && x_diff > z_diff) major_axis = X_AXIS;
	else if (  y_diff > x_diff && y_diff > z_diff) major_axis = Y_AXIS;
	else if (z_diff > x_diff && z_diff > y_diff) major_axis = Z_AXIS;

	++axis_count[major_axis];
}

int find_largest( int a, int b, int c){
	if ( a > b && a > c) return X_AXIS;
	else if (  b > a && b > c) return Y_AXIS;
	else if (c > a && c > b) return Z_AXIS;
	return Z_AXIS;
}

//input: steps taken in 20 ft dist
//output: average stride (in meters)
float determine_stride(int localSteps){
	float meters = 6.096;
	return (float)(meters/localSteps);
}


