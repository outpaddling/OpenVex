#include <stdio.h>
#include "timer.h"
#include "io.h"
#include "accelerometer.h"

/**
 * \defgroup accelerometer  Accelerometer functions
 *  @{
 *
 *  These functions provide a high-level interface to the Vex accelerometer.
 *  Each axis of the accelerometer can also be read as a standard
 *  analog sensor using io_read_analog().
 */

/**
 *  This function is unfinished.  Velocity and position data are
 *  currently very unreliable.  Filtering will be needed to correct this.
 *
 *  Return the current acceleration, velocity, and position along
 *  an axis for the accelerometer on the given analog port.
 *
 *  The accelerometer sensor provides the acceleration detected along each
 *  axis to a separate analog input port.
 *
 *  The integral of acceleration (area under the acceleration curve)
 *  is velocity.  ( Acceleration is the
 *  derivative of velocity, i.e. acceleration = velocity / time. )
 *
 *  The integral of velocity (area under the velocity curve) is position.
 *  ( Velocity is the derivative of position, i.e.
 *  velocity = position / time. )
 *
 *  This function computes approximate velocity and position by summing
 *  acceleration * dt and velocity * dt, respectively.  The dt term is the
 *  time of the function call minus the time of the previous call.
 *
 *  Since the computed area for each slice is a trapezoid that only
 *  approximates the area under the curve, a larger dt will produce greater
 *  errors. Hence, the more often this function is called, the better the
 *  approximations of velocity and position.  ( Consult the introduction to
 *  integration in any calculus text for a detailed explanation with
 *  pretty pictures and everything. )
 *
 *  Since the velocity and position are sums of approximate slice areas,
 *  their inaccuracy increases over time.  This makes minimizing dt
 *  all the more important.
 *
 *  The information returned by this function can use used in a PID
 *  loop to maintain position or velocity, for collision detection,
 *  tilt detection, etc.
 *
 *  \param  port        Analog port to which accelerometer is attached.
 *  \param  base_val    The value returned for this axis when no
 *                      acceleration is present.  This parameter is
 *                      used to calibrate so that acceleration in both
 *                      directions is measured equally.
 *  \param  scaling_factor  Value to divide velocity and position by
 *                          before returning them.  You should determine
 *                          this value by trial and error.  It depends
 *                          how much precision you want in these terms
 *                          and how large you want to allow them to grow.
 *  \param  acceleration Address of int variable to receive acceleration.
 *  \param  velocity    Address of int variable to receive velocity.
 *  \param  position    Address of int variable to receive position.
 */

/*
 *  We need about 10000 samples from the accelerometer to cancel out
 *  motor vibration using simple average.  This takes about 1/2 second
 *  to aquire.  On to a better filtering technique...
 */

/***************************************************************************
 * History: 
 *  Dec 2009    J Bacon
 ***************************************************************************/

void    read_accelerometer_axis(unsigned char port, int base_val,
			    int scaling_factor, int *acceleration,
			    long *velocity, long *position)

{
    static unsigned long    timer,
			    old_timer;
    static long             dt,
			    old_velocity = 0L,
			    raw;
    static int              old_acceleration = 0,
			    c;
    static unsigned int     calls = 0;

    /*
     *  Auto-scale dt to match frequency of calls to avoid integer overflows?
     */
    
    timer = timer0_read32();
    dt = (timer - old_timer);
    old_timer = timer;

    /* 
     *  Lowest bits tend to be noisy, so discard them.  Don't use shifting
     *  here, since right-shifting a negative 2's comp value will never
     *  get to 0, hence the +/- sides won't balance.
     *
     *  Max possible acceleration is 0x200 / 4 = 2048. Hence, 
     *  max dt = 2^31 (LONG_MAX) / 2048 = 1048576, about 1/10 second
     *  to avoid integer overflow.
     *
     *  dt should be much smaller to make an accelerometer useful for
     *  speed and distance computations.
     */
    raw = 0;
    for (c=0; c < SAMPLE_SIZE; ++c)
	raw += (int)io_read_analog(port) - base_val;
    raw /= SAMPLE_SIZE;
    *acceleration = raw / 8;

    /* velocity is integral of acceleration */
    *velocity += (*acceleration + old_acceleration) * dt / 2 / scaling_factor;
    
    /* position is integral of velocity */
    *position += (*velocity + old_velocity) * dt / 2 / scaling_factor;

    old_acceleration = *acceleration;
    old_velocity = *velocity;
    
    printf("raw = %ld  accel = %d  velo = %ld  pos = %ld\n",
	    raw, *acceleration, *velocity, *position);
}

/** @} */

