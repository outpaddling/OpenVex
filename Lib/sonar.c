/**************************************************************************
*  
*   Functions for handling the Vex ultrasonic range finder (sonar).
*   
*   WARNING: These functions configure Timer0 to run continuously and
*   and will not function correctly if other code alters Timer0 settings
*   or resets the Timer0 value.  If your program uses a sonar sensor,
*   it should treat Timer0 as read-only.
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
 *  \defgroup sonar Ultrasonic Sensors (Sonar)
 *  @{
 *
 *  These functions provide an interface to the ultrasonic sensor (sonar)
 *  which emits and ultrasonic pulse and detects an echo from nearby
 *  objects.  The time between the pulse emission and echo detection
 *  determines the distance of the object.
 */

#include <stdio.h>
#include "platform.h"
#include "vex_usart.h"
#include "io.h"
#include "timer.h"
#include "interrupts.h"
#include "debug.h"
#include "sonar.h"

extern unsigned char           Sonar_on_iport[6];
extern unsigned char           Sonar_output_port[6];
extern volatile unsigned char  Sonar_data_available[6];
extern volatile unsigned short Sonar_echo_time[6];
extern volatile unsigned short Timer0_at_emit[6];
extern volatile unsigned short Timer0_at_echo[6]; 
extern unsigned char    Analog_ports;

/**
 *  Initialize the sonar system for synchronous operation on two
 *  digital I/O ports.
 *
 *  \param  output_port Digital output port to which the pulse is sent
 *  \param  input_port  Digital input port from which the echo is read
 *
 *  \returns    OV_OK on success, OV_BAD_PARAM if either port is invalid
 *
 *  This is a minimalistic function which exists primarily to demonstrate
 *  and test ultrasound sensor operation, but also provides the ability to
 *  use a sonar sensor in the unlikely event that all interrupt ports are
 *  in use by other sensors.  The ultrasound input
 *  terminal should be plugged into a digital output port, and the
 *  ultrasound output to a digital input port.
 *
 *  Note that this is NOT an efficient way to handle ultrasound input.
 *  This function busy waits (sits in a loop) waiting for the echo return.
 *  Since sound travels through air at a rate of about 29us/cm, each
 *  round trip to/from an object 1 meter away will take about 5.8ms, which
 *  is 58,000 instruction cycles at 10 MIPS.  Put another way, if you
 *  call this function once for each iteration of the master data
 *  processing loop, which takes about 18.5ms, almost 1/3 of your available
 *  CPU time for each iteration will go down the drain.
 *
 *  Using the interrupts-based interface (sonar_init() and sonar_read())
 *  is highly recommended, unless
 *  your program can afford to waste a lot of CPU time.
 */

/*
 * History:
 *  Dec 2008    J Bacon
 */

status_t    sonar_init_spin_wait(unsigned char output_port,
				 unsigned char input_port)

{
    if ( ! VALID_DIGITAL_PORT(input_port) ||
	 ! VALID_DIGITAL_PORT(output_port) )
	return OV_BAD_PARAM;

    io_set_direction(output_port,IO_DIRECTION_OUT);
    io_write_digital(output_port,0);
    io_set_direction(input_port,IO_DIRECTION_IN);
    return OV_OK;
}


/**
 *  This is a minimalistic function which exists primarily to demonstrate
 *  and test ultrasound sensor operation, but also provides the ability to
 *  use a sonar sensor in the unlikely event that all interrupt ports are
 *  in use by other sensors.  The ultrasound input
 *  terminal should be plugged into a digital output port, and the
 *  ultrasound output to a digital input port.
 *
 *  \param  output_port Digital output port to which the pulse is sent
 *  \param  input_port  Digital input port from which the echo is read
 *
 *  \returns    Distance in centimeters
 *
 *  Note that this is NOT an efficient way to handle ultrasound input.
 *  This function busy waits (sits in a loop) waiting for the echo return.
 *  Since sound travels through air at a rate of about 29us/cm, each
 *  round trip to/from an object 1 meter away will take about 5.8ms, which
 *  is 58,000 instruction cycles at 10 MIPS.  Put another way, if you
 *  call this function once for each iteration of the master data
 *  processing loop, which takes about 18.5ms, almost 1/3 of your available
 *  CPU time for each iteration will go down the drain.
 *
 *  Using the interrupts-based interface (sonar_init() and sonar_read())
 *  is highly recommended, unless
 *  your program can afford to waste a lot of CPU time.
 */

/*
 * History:
 *  Dec 2008    J Bacon
 */

unsigned int    sonar_read_spin_wait(
			unsigned char output_port,
			unsigned char digital_in_port)

{
    unsigned int    timer0_at_echo,
		    timer0_at_emit,
		    sonar_echo_time;

    /* Send start signal to ultrasonic range finder */
    //DPRINTF("Sending pulse...\n");
    sonar_emit_pulse(output_port);

    /*
     *  Now wait for the response from the sonar.  It will send a high
     *  signal (1) to the digital input port when the sonic pulse has
     *  been sent, and a low signal when it detects an echo.
     *
     *  Note: io_write_digital() and io_read_digital() both take a few
     *  timer cycles, but since it's the difference between time points
     *  we're interested in, this won't affect anything.
     */
    
    /* Wait for sonar output to rise, indicating the pulse has been sounded */
    //DPRINTF("Waiting for emit signal...\n");
    while ( io_read_digital(digital_in_port) == 0 )
	;

    TIMER0_READ16(timer0_at_emit);
    
    /* When the sonar detects an echo, it will drop its output */
    //DPRINTF("Waiting for echo...\n");
    while ( io_read_digital(digital_in_port) == 1 )
	;
    
    TIMER0_READ16(timer0_at_echo);
    
    sonar_echo_time = timer0_at_echo - timer0_at_emit;

    //DPRINTF("emit = %u  echo = %u\n", timer0_at_emit, timer0_at_echo);

    /* Rough estimate of centimeters */
    return ECHO_TIME_TO_CM(sonar_echo_time);
}


/**
 *  Initialize the sonar system for async operation using a
 *  digital I/O port to trigger the sensor pulse, and an interrupt port
 *  to recive the echo response.
 */

/*
 * History:
 *  Dec 2008    J Bacon
 */

status_t    sonar_init(unsigned char interrupt_port,
		       unsigned char output_port)

{
    if ( ! VALID_INTERRUPT_PORT(interrupt_port) ||
	 ! VALID_DIGITAL_PORT(output_port) )
	return OV_BAD_PARAM;
    
    io_set_direction(output_port,IO_DIRECTION_OUT);
    io_write_digital(output_port,0);
    SET_SONAR_ON_IPORT(interrupt_port);
    Sonar_output_port[interrupt_port-1] = output_port;
    sonar_emit_pulse_interrupt(output_port,interrupt_port);
    return OV_OK;
}


/**
 *  Returns the approximate distance (in centimeters) of an object
 *  in front of the ultrasonic sensor (sonar) using the interrupt-driven
 *  sonar driver initialized by sonar_init().  The sonar_read() function
 *  also uses the output port associated with interrupt_port by
 *  sonar_init().
 *
 *  Note that the sonar device operates by sending ultrasonic pulses
 *  and waiting for echos.  This process is significantly slower
 *  than the Vex processor's instruction cycle.  Hence, it will often
 *  be the case that most calls to sonar_read() will not find
 *  any new data from the sensor available.
 *
 *  \param  interrupt_port  Interrupt port to which the sonar is attached
 *
 *  \returns    Distance in centimeters
 *
 *  When new data is not available, sonar_read() simply returns
 *  the same value as the previous call.  In other words, it always returns
 *  the most recent available sample from the sonar sensor.
 */

/*
 * History:
 *  Dec 2008     J Bacon
 */

unsigned int    sonar_read(unsigned char interrupt_port)

{
    /* Multiple sonar sensors could be connected to any port. */
    static char         waiting_for_echo[6] = {0, 0, 0, 0, 0, 0};
    static unsigned int last_distance[6] = {0, 0, 0, 0, 0, 0};    
    unsigned short      t0,
			echo_time;
    
    if ( ! VALID_INTERRUPT_PORT(interrupt_port) )
	return OV_BAD_PARAM;

    /*
     *  Send the next pulse after an echo or timeout is received. 
     */
    if ( !waiting_for_echo[interrupt_port-1] )
    {
	/*
	 *  Wait about 14ms before emitting next pulse.  Not sure
	 *  why yet. Property of sonar hardware?
	 */
	TIMER0_READ16(t0);
	/*
	 *  Investigate:
	 *  Sonar gets stuck if a smaller wait time is used.
	 *  It never reports data available, so the next
	 *  pulse never gets sent.
	 */
	echo_time = (t0 - Timer0_at_echo[interrupt_port-1]) / TIMER0_TICKS_PER_MS;
	if ( echo_time > 11 )
	{
	    sonar_emit_pulse_interrupt(Sonar_output_port[interrupt_port-1],
		interrupt_port);
	    waiting_for_echo[interrupt_port-1] = 1;
	}
    }
    /* Got echo return interrupt? */
    else if ( SONAR_DATA_AVAILABLE(interrupt_port) )
    {
	CLR_SONAR_DATA_AVAILABLE(interrupt_port);
	
	/* Indicate that another pulse must be sent as soon as possible */
	waiting_for_echo[interrupt_port-1] = 0;
	
	/* Crude conversion of echo time to centimeters */
	last_distance[interrupt_port-1] = 
	    ECHO_TIME_TO_CM(SONAR_ECHO_TIME(interrupt_port));
    }
    return last_distance[interrupt_port-1];
}


/****************************************************************************
 *  Send a start signal to the ultrasonic range finder causing it to
 *  emit an ultrasonic pulse.
 *
 * History:
 *  Dec 2008     J Bacon
 ***************************************************************************/

status_t    sonar_emit_pulse(unsigned char output_port)

{
    unsigned char   start, end;
    
    /*
     *  Send start signal to sonar in the form of a 13us high pulse.
     *  TMR0IF will be set when the timer overflows even though an
     *  interrupt will not be generated.
     */

    if ( ! VALID_DIGITAL_OUTPUT_PORT(output_port) )
	return OV_BAD_PARAM;

    io_write_digital(output_port,1);    /* Start pulse. */
    start = TMR0L;
    // FIXME: End value should be linked to prescale factor
    end = start + 16;
    while (TMR0L < end)
	;
    io_write_digital(output_port,0);    /* End pulse. */
    //DPRINTF("Pulsed from %u to %u\n",start,end);
    return OV_OK;
}


/**************************************************************************
 *  Emit a sonar pulse and enable interrupts.
 *
 * History:
 *  Dec 2008     J Bacon
 **************************************************************************/

status_t    sonar_emit_pulse_interrupt(unsigned char output_port,
				       unsigned char interrupt_port)

{
    /*
     *  Catching an interrupt on the rising edge will tell us when the
     *  sonic pulse has been sent, since the ultrasonic sensor transitions
     *  it's digital output to 1 at that time.  When the echo is received,
     *  the sensor will drop its output to 0, so we'll want a trailing
     *  edge interrupt source at that time.
     */

    if ( ! VALID_INTERRUPT_PORT(interrupt_port) ||
	 ! VALID_DIGITAL_OUTPUT_PORT(output_port) )
	return OV_BAD_PARAM;

    interrupt_set_edge(interrupt_port,INTERRUPT_RISING_EDGE);
    interrupt_enable(interrupt_port);
    sonar_emit_pulse(output_port);
    return OV_OK;
}

/** @} */

