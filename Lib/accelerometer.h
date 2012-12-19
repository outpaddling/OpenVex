
#ifndef __accelerometer_h__
#define __accelerometer_h__

#define SAMPLE_SIZE 10000

void    read_accelerometer_axis(unsigned char port, int base_val,
			    int scaling_factor, int *acceleration,
			    long *velocity, long *position);

#endif

