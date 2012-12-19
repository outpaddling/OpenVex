/**************************************************************************
* Description: 
*   Routines for handling shaft_encoder input.
*   
***************************************************************************
*
*   This code is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*               
*   This code is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this code.  If not, see <http://www.gnu.org/licenses/>.
*
***************************************************************************
*   History:
*       Created Dec 2008    J Bacon
***************************************************************************/

/**
 *  \defgroup encoder Shaft Encoder
 *  @{
 *
 *  These functions provide support for the shaft encoder, which is used
 *  to determine the rotational velocity of an axle.
 */

#include <stdio.h>
#include "platform.h"
#include "interrupts.h"
#include "timer.h"
#include "master.h"
#include "general.h"
#include "shaft_encoder.h"
#include "io.h"
#include "debug.h"

extern volatile unsigned int    Encoder_ticks[6];
extern unsigned char            Encoder_on_iport[6];
extern unsigned char            Quad_input_port[6];
extern unsigned char            Analog_ports;

/**
 *  Enable a standard optical shaft encoder on the specified interrupt
 *  port.
 *
 *  \param  interrupt_port  Interrupt port to which encoder is attached.
 *
 *  \returns    OV_OK on success, OV_BAD_PARAM if port is invalid.
 */

/*
 * History:
 *  Dec 2008     J Bacon
 */

status_t    shaft_encoder_enable_std(unsigned char interrupt_port)

{
    status_t    stat;
    
    if ( (stat = shaft_encoder_enable(interrupt_port)) == OV_OK )
	SET_ENCODER_ON_IPORT(interrupt_port, ENCODER_STD);
    return stat;
}


/**
 *  Enable a quadrature optical shaft encoder on the specified interrupt
 *  port, and input port.
 *
 *  \param  interrupt_port  Interrupt port to which shaft encoder is attached.
 *  \param  input_port      Digital input port to which secondary shaft
 *                          encoder cable is attached.
 *
 *  \returns    OV_OK on success, OV_BAD_PARAM if either port is invalid
 *              or if input_port is not configured for digital input.
 *
 *  The input port must be configured
 *  for digital input.  ( See io_set_analog_port_count() and
 *  io_set_direction(). )
 */

/*
 * History:
 *  Dec 2008     J Bacon
 */

status_t    shaft_encoder_enable_quad(unsigned char interrupt_port,
	    unsigned char input_port)

{
    status_t    stat;
    
    if ( ! VALID_INTERRUPT_PORT(interrupt_port) ||
	 ! VALID_DIGITAL_INPUT_PORT(input_port) )
	return OV_BAD_PARAM;
    
    if ( (stat = shaft_encoder_enable(interrupt_port)) == OV_OK)
    {
	SET_ENCODER_ON_IPORT(interrupt_port, ENCODER_QUAD);
	Quad_input_port[(interrupt_port)-1] = input_port;
    }
    return stat;
}


status_t    shaft_encoder_enable(unsigned char interrupt_port)

{ 
    if ( ! VALID_INTERRUPT_PORT(interrupt_port) ||
	  INTERRUPT_PORT_IN_USE(interrupt_port) )
	return OV_BAD_PARAM;
    
    SET_INTERRUPT_PORT_IN_USE(interrupt_port);
    Encoder_ticks[interrupt_port-1] = 0;

    switch(interrupt_port)
    {
	case    1:
	    /* Enable interrupt port 1 */
	    INTCON2bits.INTEDG2 = 1;  /* INTERRUPT_IN1 rising edge triggered */
	    INTCON3bits.INT2IP = 0;   /* INTERRUPT_IN1 low priority */
	    INTCON3bits.INT2IF = 0;   /* Clear flag for INTERRUPT_IN1 */
	    INTCON3bits.INT2IE = 1;   /* Enable INTERRUPT_IN1 */
	    break;
	case    2:
	    /* Enable interrupt port 2 */
	    INTCON2bits.INTEDG3 = 1;  /* INTERRUPT_IN2 rising edge triggered */
	    INTCON2bits.INT3P = 0;    /* INTERRUPT_IN2 low priority */
	    INTCON3bits.INT3IF = 0;   /* Clear flag for INTERRUPT_IN2 */
	    INTCON3bits.INT3IE = 1;   /* Enable INTERRUPT_IN2 */
	    break;
	case    3:
	case    4:
	case    5:
	case    6:
	    /* Ports 3 - 6 all use RB interrupts */
	    /* Triggers on any change (either edge) */
	    INTCON2bits.RBIP = 0;   /* Low priority */
	    INTCONbits.RBIF = 0;    /* Clear interrupt flag */
	    INTCONbits.RBIE = 1;    /* Enable interrupts */
	    break;
    }
    return OV_OK;
}


/**
 *  Disable the standard or quadrature shaft encoder on the specified
 *  interrupt port.  This will prevent the encoder from generating
 *  further interrupts and updating the rotation counter.
 *
 *  \param  interrupt_port  The interrupt port to which the encoder is attached.
 *
 *  \returns    OV_OK on success, OV_BAD_PARAM if the port is invalid.
 */

/*
 * History:
 *  Dec 2008     J Bacon
 */

status_t    shaft_encoder_disable(unsigned char interrupt_port)

{
    int     count,
	    p;

    if ( ! VALID_INTERRUPT_PORT(interrupt_port) )
	return OV_BAD_PARAM;
    
    CLR_INTERRUPT_PORT_IN_USE(interrupt_port);
    CLR_ENCODER_ON_IPORT(interrupt_port);
    
    switch(interrupt_port)
    {
	case    1:
	    INTCON3bits.INT2IE = 0; /* Disable INTERRUPT_IN2 */
	    break;
	case    2:
	    INTCON3bits.INT3IE = 0; /* Disable INTERRUPT_IN2 */
	    break;
	case    3:
	case    4:
	case    5:
	case    6:
	    /*
	     *  The RB interrupt is shared by all these ports.  Disable it
	     *  only if none of them are still using it.
	     */
	    count = 0;
	    for (p=3; p<6; ++p)
		count += INTERRUPT_PORT_IN_USE(p);
	    if ( count == 0 )
	    {
		INTCONbits.RBIE = 0;    /* Disable RB interrupts */
	    }
	    break;
    }
    return OV_OK;
}


/**
 *  Manually reset encoder rotation counter associated with the
 *  given interrupt port.
 *
 *  \param  interrupt_port  The interrupt port to which the encoder is attached.
 *
 *  \returns    OV_OK on success, OV_BAD_PARAM if the port is invalid.
 */

/*
 * History:
 *  Dec 2008     J Bacon
 */

status_t    shaft_encoder_reset(unsigned char interrupt_port)

{
    if ( ! VALID_INTERRUPT_PORT(interrupt_port) )
	return OV_BAD_PARAM;
    
    Encoder_ticks[interrupt_port-1] = 0;
    return OV_OK;
}


/**
 *  Return the current encoder tick count associated with the specified
 *  interrupt port.  Also available as the macro SHAFT_ENCODER_READ_STD()
 *  where the global Encoder_ticks[] is visible.
 *
 *  \param  interrupt_port  The interrupt port to which the encoder is attached.
 *
 *  \returns    The number of shaft encoder ticks since the encoder
 *              was last initialized or reset.
 */

/*
 * History:
 *  Dec 2008     J Bacon
 */

unsigned int   shaft_encoder_read_std(unsigned char interrupt_port)

{
    if ( ! VALID_INTERRUPT_PORT(interrupt_port) )
	return OV_BAD_PARAM;

    return Encoder_ticks[interrupt_port-1];
}


/**
 *  Return the current quadrature encoder tick count associated with
 *  the specified interrupt port.  Also available as the macro
 *  SHAFT_ENCODER_READ_QUAD() where the global Encoder_ticks[] is visible.
 *
 *  The shaft_encoder_read_quad() function also uses the input port
 *  associated with interrupt_port by shaft_init_quad().
 *
 *  \param  interrupt_port  The interrupt port to which the encoder is attached.
 *
 *  \returns    The number of shaft encoder ticks since the encoder
 *              was last initialized or reset.
 */

/*
 * History:
 *  May 2009    J Bacon
 */

int     shaft_encoder_read_quad(unsigned char interrupt_port)

{
    if ( ! VALID_INTERRUPT_PORT(interrupt_port) )
	return OV_BAD_PARAM;

    return (int)Encoder_ticks[interrupt_port-1];
}


/**
 *  Attempt to maintain up to ENCODER_MAX_SHAFTS shaft encoders at individual set-points
 *  for ticks per second (TPS) over a given period of time or total
 *  tick count.
 *  
 *  Note:
 *
 *  \code
 *  Ticks   rotations   90 ticks     1 minute
 *  ----- = --------- x ---------- x --------
 *  sec     minute      1 rotation   60 sec
 *  \endcode
 *
 *  The maximum RPMs for VEX shaft encoders is 1133 (1699 TPS).
 *  The maximum motor speed is 100 RPM (150 TPS).  Hence a gearing
 *  ratio from motor to shaft encoder of more than 11.33 to 1
 *  may result in unreliable encoder feedback.
 *
 *  This function will do a reasonable job of maintaining RPMS of
 *  multiple shafts under normal conditions.  This can be used to make
 *  a robot track along a straight line or constant arc, for example.
 *
 *  Results are undefined when the motors are not capable of achieving
 *  necessary RPMs.  This could be caused by selecting a TPS value
 *  beyond the motor's maximum free speed, or by friction or other
 *  impedence.
 *
 *  Clutches are recommended to prevent motor damage for all applications
 *  where physical impedence is possible.
 *
 *  \param  shafts  Array of shaft_t structures, initialized before
 *                  calling shaft_tps_run() using shaft_tps_init()
 *  \param  count   Number of shafts to maintain
 *
 *  Example: Run a 2-motor skid steer robot in a straight line for 3
 *  complete shaft rotations.
 *
 *  \code
 *  #define TIME_LIMIT                      0   // 0 means no time limit
 *  #define TICKS_PER_ROTATION              90
 *  #define LEFT_MOTOR_PORT                 3
 *  #define RIGHT_MOTOR_PORT                4
 *  #define LEFT_ENCODER_INTERRUPT_PORT     1
 *  #define RIGHT_ENCODER_INTERRUPT_PORT    4
 *  #define DESIRED_TPS                     60
 *
 *  shaft_t shafts[2];
 *
 *  // Send 0 for I/O port to indicate non-quadrature encoder
 *  shaft_tps_init(&shafts[0], TIME_LIMIT, TICKS_PER_ROTATION * 3,
 *      LEFT_DRIVE_PORT, LEFT_ENCODER_INTERRUPT_PORT, 0, -DESIRED_TPS);
 *  shaft_tps_init(&shafts[1], TIME_LIMIT, TICKS_PER_ROTATION * 3,
 *      RIGHT_DRIVE_PORT, RIGHT_ENCODER_INTERRUPT_PORT, 0, DESIRED_TPS);
 *  shaft_tps_run(shafts, 2);
 *  \endcode
 */

/***************************************************************************
 * History: 
 *  Nov 2009    J Bacon
 * FIXME: Does not yet handle quad encoders properly.
 ***************************************************************************/

status_t    shaft_tps_run(shaft_t shafts[], unsigned char count)

{
    static char             active_timers,
			    active_counters;
    static shaft_t          *sp;
    static unsigned long    start_time,
			    elapsed_time;
    
    /*
     *  PID tuning constants (gains)
     *      power += kp * error + ki * error-sum + kd * error-derivitive
     *
     *  Each gain k* is separated into numerator k*n and denominator
     *  k*d to allow fractional gains without resorting to the use
     *  of floating point.  Floating point generally results in a larger,
     *  slower program, and on many processors increases power consumption.
     *
     *  If you increase sample_interval, you should decrease the gain
     *  values proportionally, since they'll be applied less often.
     */
    
    static short            kpn = 1,
			    kpd = 12,
			    kin = 0,
			    kid = 1,
			    kdn = 1,
			    kdd = 1,
			    sample_interval = 50;   /* Milliseconds */

    static unsigned long    actual_ticks,
			    expected_ticks;
    short                   new_power,
			    proportional,
			    derivative,
			    error;
		
    if ( ! VALID_ENCODER_COUNT(count) )
	return OV_BAD_PARAM;
    
    /* Get starting time in ms */
    start_time = TIMER0_ELAPSED_MS;
    
    /* Reset shaft encoder counters */
    for (sp = shafts; sp < shafts + count; ++sp)
    {
	if ( ENCODER_ON_IPORT(sp->interrupt_port) )
	{
	    shaft_encoder_reset(sp->interrupt_port);
	    sp->leave_encoder_on = 1;
	}
	else
	{
	    shaft_encoder_enable(sp->interrupt_port);
	    sp->leave_encoder_on = 0;
	}
	sp->power = 0;
	sp->integral = 0;
    }
    
    /*
     *  Run an ideal PID loop until all time/counter limits are reached.
     */
    
    do
    {
	active_timers = active_counters = count;
	
	/* Get elapsed time in ms */
	elapsed_time = TIMER0_ELAPSED_MS - start_time;
	
	/* Run PID controls at regular intervals */
	if ( elapsed_time % sample_interval == 0 )
	{
	    /*
	     *  Do the more expensive calculation in the initializer
	     *  and the simple one in the comparison to save a few
	     *  cycles in each iteration.
	     */
	    for (sp = shafts + count - 1; sp >= shafts; --sp)
	    {
		/* Check timer and counter */
		if ( elapsed_time >= sp->timer_limit )
		    --active_timers;
		
		actual_ticks = SHAFT_ENCODER_READ_STD(sp->interrupt_port);
		if ( actual_ticks >= sp->tick_limit )
		    --active_counters;
		
		/* Where should we be at this time? */
		expected_ticks = ABS(sp->tps) * elapsed_time / MS_PER_SEC;
		
		error = expected_ticks - actual_ticks;
		
		/* Proportional adjustment = kp * error */
		proportional = kpn * error / kpd;
		
		/*
		 *  Integral (sum of previous errors) accelerates big
		 *  adjustments but can also increase overshoot, so use
		 *  a cheap method (multiply by 2/3) to exponentially
		 *  decay the weight of old errors along the way.
		 */
		sp->integral = (sp->integral * 2 / 3 + (kin * error)) / kid;
		
		/* Derivative */
		derivative = kdn * (error - sp->previous_error) / kdd;
		sp->previous_error = error;
		
		/* Adjust power */
		new_power = ABS(sp->power) + 
		    proportional + sp->integral + derivative;
		sp->power = new_power > 127 ? 127 :
		    (new_power < 0 ? 0 : new_power);
		if ( sp->tps < 0 )
		    sp->power = -sp->power;
	    
		DPRINTF("t: %6lu  m: %u  et: %ld  at: %ld  p: %d  i: %d  d: %d  np: %d %d\n",
			elapsed_time, sp->motor_port,
			expected_ticks, actual_ticks, proportional,
			sp->integral, derivative, new_power, sp->power);
		pwm_write(sp->motor_port, sp->power);
	    }
	    controller_submit_data(WAIT);
	}
    }   while ( active_timers || active_counters );
    
    for (sp = shafts; sp < shafts + count; ++sp)
	if ( !sp->leave_encoder_on )
	    shaft_encoder_disable(sp->interrupt_port);
    return OV_OK;
}


/**
 *  Initialize a shaft_t structure for shaft_tps_run()
 *
 *  \param  sp          Pointer to element in shaft_t array
 *  \param  timer_limit Number of milliseconds to control this shaft
 *  \param  tick_limit  Total number of ticks this shaft should
 *                      rotate under control
 *  \param  motor_port  PWM port number of motor controlling this shaft
 *  \param  interrupt_port
 *                      Interrupt port to which shaft encoder is attached
 *  \param  input_port
 *                      Quadrature shaft encoders only.  Secondary I/O
 *                      port to which quad shaft encoder is attached.
 *                      For standard encoders, pass 0 to this parameter.
 *  \param  ticks_per_second
 *                      Desired rotational speed for the shaft.
 */

/***************************************************************************
 * History: 
 *  Nov 2009    J Bacon
 ***************************************************************************/

status_t    shaft_tps_init(shaft_t *sp,
		    unsigned long timer_limit, unsigned short tick_limit,
		    unsigned char motor_port, unsigned char interrupt_port,
		    unsigned char input_port, short ticks_per_second)

{
    printf("%d %d %d\n", motor_port, interrupt_port, input_port);
    if ( ! VALID_PWM_PORT(motor_port) ||
	 ! VALID_INTERRUPT_PORT(interrupt_port) ||
	 ((input_port != 0) && ! VALID_DIGITAL_PORT(input_port)) )
	return OV_BAD_PARAM;
    
    io_set_direction(input_port, IO_DIRECTION_IN);
    
    sp->timer_limit = timer_limit;
    sp->tick_limit = tick_limit;
    sp->motor_port = motor_port;
    sp->interrupt_port = interrupt_port;
    sp->input_port = input_port;
    sp->tps = MIN(ticks_per_second, 1699);
    
    return OV_OK;
}

/** @} */

