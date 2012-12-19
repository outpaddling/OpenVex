#include <stdio.h>
#include <math.h>
#include "general.h"
#include "arcade_drive.h"

/**
 * \defgroup arcade Arcade Drive
 *  @{
 *
 *  These functions provide a convenient interface for arcade
 *  drive programs, where a single joystick is used to maneuver the
 *  robot (as opposed to tank drive, where two joysticks are used).
 */

/**
 *  Convert joystick x and y coordinates (-JOY_MAX to +JOY_MAX) to left and
 *  right motor speed (-power_max to power_max) for use in skid
 *  (differential) steering.
 *
 *  \param      joy_x   Joystick horizontal position
 *  \param      joy_y   Joystick vertical position
 *  \param      left_power_ptr  Address of variable to receive left motor power
 *  \param      right_power_ptr Address of variable to receive right motor power
 *  \param      power_max       Maximum absolute value for motor power (up
 *              to 127 on Vex)
 *  \returns    OV_OK on success, OV_BAD_PARAM on failure
 *
 *  Joystick position to power mapping:
 *
 *  Below is a partial map of the joystick Cartesian plane, showing the desired
 *  left and right motor speeds (left,right) for each position on the
 *  plane.  This map was designed by first defining vehicle behavior at
 *  the center position (x,y = 0,0) and at the
 *  extremes, where x or y is -JOY_MAX or JOY_MAX.  Power values between are
 *  linearly interpolated along the x and y axes (not diagonally).
 *
 *  Note: Linear interpolation does not work well for all systems.  For
 *  example, the nxtremote command for remote control of Lego NXT robots
 *  (part of roboctl) warps the mapping of joystick to power using 4th root
 *  to compensate for the large dead-band of Lego NXT motors (it takes
 *  about 30% power to get a typical robot to start moving at all).
 *  See the source code for details.
 *  Linear mapping works well for Vex robots, however.
 *
 *  For example, assuming JOY_MAX = 100:
 *
    \verbatim
    x,y = 0,0:      Vehicle is motionless.
    x,y = 0,100:    Full speed straight ahead.
    x,y = 100,0:    Full speed turning clockwise about center.
		    (Left motor full speed ahead, right full speed reverse)
    x,y = 100,100:  Full speed turning about right wheel/track.
		    (Left motor full speed, right motor stopped.)
    all x>0:        Vehicle is turning clockwise.
    all x<0:        Vehicle is turning counter-clockwise.

	0,100       50,100      100,100     100,50      100,0
    
	-50,100     0,75        50,50       75,0        100,-50
    
	-100,100    -50,50      0,0         50,-50      100,-100
    
	-100,50     -75,0       -50,-50     0,-75       50,-100
    
	-100,0      -100,-50    -100,-100   -50,-100    0,-100
    \endverbatim
 *
 *  The formula for any single quadrant is simple.  A general formula for
 *  the entire plane would be ugly.  ( Observe how the value of "left"
 *  varies across
 *  the top of the plane (where y=100), increasing from 0 to 100 over x from
 *  -100 to 0, and then becoming a constant 100 for x from 0 to 100. )
 *  
 *  However, the values are highly symmetric between adjacent quadrants.
 *  left and right are swapped going from any x to -x, and swapped and negated
 *  for any y and -y.  Hence, we start by computing (left,right) for
 *  (|x|,|y|) using the simple formula for quadrant 1, and then for:\n
 *
 *  2nd quadrant: swap\n
 *  3rd quadrant: negate\n
 *  4th quadrant: swap and negate\n
 */

/***************************************************************************
 * History: 
 *  June 2009   J Bacon
 ***************************************************************************/

status_t    arcade_drive(char joy_x, char joy_y, char power_max,
		    char *left_power_ptr, char *right_power_ptr)

{
    int     left_power,
	    right_power,
	    abs_joy_x = ABS(joy_x),
	    abs_joy_y = ABS(joy_y);

    /* No need to validate power_max, since it's range limits it */
    if ( (ABS(joy_x) > JOY_MAX) || (ABS(joy_y) > JOY_MAX) ||
	 ! VALID_PWM_VAL(power_max) )
	return OV_BAD_PARAM;

    /*
     * Compute left and right motor speeds using simple formula for
     * quadrant 1.
     */
    left_power = abs_joy_y + abs_joy_x * ( JOY_MAX - abs_joy_y ) / JOY_MAX;
    left_power = left_power * power_max / JOY_MAX;
    right_power = (abs_joy_y - abs_joy_x);
    right_power = right_power * power_max / JOY_MAX;
    
    if ( joy_y >= 0 )   /* Quadrants 1 and 2 */
    {
	/* 
	 *  Left speed and right speed are simply swapped when going from
	 *  joy_x,joy_y to -joy_x,joy_y.  ( Quadrant 1 to 2 )
	 */
	if ( joy_x < 0 )    /* Swap for quadrant 2 */
	    SWAP(&left_power,&right_power);
    }
    else    /* Quadrants 3 and 4 */
    {
	/*
	 *  Left speed and right speed are swapped AND negated when going
	 *  from joy_x,joy_y to joy_x,-joy_y.  ( Quadrant 1 to 4 )
	 *
	 *  Going from joy_x,joy_y to -joy_x,-joy_y (quadrant 1 to 3), motor speeds are
	 *  only negated.
	 */
	if ( joy_x >= 0 )   /* Swap for quadrant 4 only */
	    SWAP(&left_power,&right_power);

	/* Negate for quadrants 3 and 4 */
	left_power = -left_power;
	right_power = -right_power;
    }
    *left_power_ptr = left_power;
    *right_power_ptr = right_power;
    return OV_OK;
}

/** @} */

