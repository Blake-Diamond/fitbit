

#include "lab3a.h"

#define X_AXIS 0
#define Y_AXIS 1
#define Z_AXIS 2
#define SAMPLE_WINDOW 50 //100 is best so far ~8 seconds

float local_min_acc_z;
float local_max_acc_z;
float acc_thresh_z;

float thresh;
float accel_data[SAMPLE_WINDOW];
int major_axis;

volatile u8 init_flag;

int axis_count[3];

int steps; //number of steps taken
float stride;

float magXY;
char *str;

int last_step;
volatile int cal_set;
volatile u8 magCount;

/* function definitions */
void calibrate_acc_threshold(void);
int determine_acc_step(void);
void determine_major_axis(void);
float determine_stride(int localSteps);

