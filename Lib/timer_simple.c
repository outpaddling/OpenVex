#include <stdio.h>
#include "platform.h"
#include "timer.h"

extern unsigned char    Timer_allocated[4];

/**
 * \defgroup timer_simple Simplified Timer Interface
 *  @{
 *
 *  These functions provide a simplified interface to the common
 *  capabilities of all the PIC 18f8520 timers.  At present, these
 *  functions are not all that useful, since SYSTEM_TIMER_TICKS(),
 *  SYSTEM_TIMER_MS(), and SYSTEM_TIMER_SECONDS() can
 *  be used to achieve most of what these functions do.  Future
 *  expansion of this simplified interface will add functionality
 *  that cannot be well served by the system timer.
 *
 *  Timer 0 is the system timer, and is reserved by OpenVex for internal
 *  use.
 *
 *  Timers 1 through 4 can be used interchangeably here.
 *
 *  Note that timers 1 and 3 are 16 bit timers, while timers 2 and 4
 *  are 8 bit.  This does not limit the capabilities of the timers, but
 *  the 8 bit timers will generate 256 times as many interrupts / sec,
 *  since they roll over every 256 ticks, vs. every 65,536 ticks for
 *  the 16 bit timers.  Hence, timers 1 and 3 will have less impact on
 *  performance.
 */

/**
 *  Return the number of milliseconds passed since the last timer_clear()
 *  on timer.
 *
 *  \param  timer   An integer from 1 to 4, inclusive, indicating which
 *                  timer to use.  The Vex PIC processor has 5
 *                  timers, one of which is reserved for use by
 *                  the OpenVex library.  The other 4 are available
 *                  to the simplified timer interface functions.
 */

unsigned long   timer_read_ms(unsigned char timer)
{
    switch(timer)
    {
	case    1:
	    return TIMER1_ELAPSED_MS;
	case    2:
	    return TIMER2_ELAPSED_MS;
	case    3:
	    return TIMER3_ELAPSED_MS;
	case    4:
	    return TIMER4_ELAPSED_MS;
    }
    return 0;
}


/**
 *  Set timer to 0.
 *
 *  \param  timer   An integer from 1 to 4, inclusive, indicating which
 *                  timer to use.  The Vex PIC processor has 5
 *                  timers, one of which is reserved for use by
 *                  the OpenVex library.  The other 4 are available
 *                  to the simplified timer interface functions.
 */

void    timer_clear(unsigned char timer)
{
    switch(timer)
    {
	case    1:
	    TIMER1_WRITE16(0);
	    Timer1_overflows = 0;
	    return;
	case    2:
	    /*
	     *  Set to 1 so we can interrupt when matching PR = 0
	     *  and Timer2_overflows will be updated when the timer
	     *  rolls over from ff to 00.
	     */
	    TMR2 = 0;   /* Clear postscale bits to get 1:1 */
	    TIMER2_WRITE8(1);
	    Timer2_overflows = 0;
	    TIMER2_WRITE_PR(0);
	    return;
	case    3:
	    TIMER3_WRITE16(0);
	    Timer3_overflows = 0;
	    return;
	case    4:
	    /*
	     *  Set to 1 so we can interrupt when matching PR = 0
	     *  and Timer2_overflows will be updated when the timer
	     *  rolls over from ff to 00.
	     */
	    TMR4 = 0;   /* Clear postscale bits to get 1:1 */
	    TIMER4_WRITE8(1);
	    Timer4_overflows = 0;
	    TIMER4_WRITE_PR(0);
	    return;
    }
}


/**
 *  Start timer running.      
 *
 *  \param  timer   An integer from 1 to 4, inclusive, indicating which
 *                  timer to use.  The Vex PIC processor has 5
 *                  timers, one of which is reserved for use by
 *                  the OpenVex library.  The other 4 are available
 *                  to the simplified timer interface functions.
 */

void    timer_start(unsigned char timer)
{
    switch(timer)
    {
	case    1:
	    TIMER1_STOP();
	    TIMER1_SET_WIDTH_16();
	    TIMER1_SET_PRESCALE(TIMER1_PRESCALE_MASK_8);
	    TIMER1_CLEAR_INTERRUPT_FLAG();
	    TIMER1_ENABLE_INTERRUPTS();
	    TIMER1_START();
	    return;
	case    2:
	    TIMER2_STOP();
	    TIMER2_SET_PRESCALE(TIMER2_PRESCALE_MASK_16);
	    TIMER2_CLEAR_INTERRUPT_FLAG();
	    TIMER2_ENABLE_INTERRUPTS();
	    TIMER2_START();
	    return;
	case    3:
	    TIMER3_STOP();
	    TIMER3_SET_WIDTH_16();
	    TIMER3_SET_PRESCALE(TIMER1_PRESCALE_MASK_8);
	    TIMER3_CLEAR_INTERRUPT_FLAG();
	    TIMER3_ENABLE_INTERRUPTS();
	    TIMER3_START();
	    return;
	case    4:
	    TIMER4_STOP();
	    TIMER4_SET_PRESCALE(TIMER4_PRESCALE_MASK_16);
	    TIMER4_CLEAR_INTERRUPT_FLAG();
	    TIMER4_ENABLE_INTERRUPTS();
	    TIMER4_START();
	    return;
    }
}


/**
 *  Stop timer from running.
 *
 *  \param  timer   An integer from 1 to 4, inclusive, indicating which
 *                  timer to use.  The Vex PIC processor has 5
 *                  timers, one of which is reserved for use by
 *                  the OpenVex library.  The other 4 are available
 *                  to the simplified timer interface functions.
 */

void    timer_stop(unsigned char timer)
{
    switch(timer)
    {
	case    1:
	    TIMER1_STOP();
	    TIMER1_CLEAR_INTERRUPT_FLAG();
	    TIMER1_DISABLE_INTERRUPTS();
	    return;
	case    2:
	    TIMER2_STOP();
	    TIMER2_CLEAR_INTERRUPT_FLAG();
	    TIMER2_DISABLE_INTERRUPTS();
	    return;
	case    3:
	    TIMER3_STOP();
	    TIMER3_CLEAR_INTERRUPT_FLAG();
	    TIMER3_DISABLE_INTERRUPTS();
	    return;
	case    4:
	    TIMER4_STOP();
	    TIMER4_CLEAR_INTERRUPT_FLAG();
	    TIMER4_DISABLE_INTERRUPTS();
	    return;
    }
}


/**
 *  Select an available timer (one that is not currently running).
 *
 *  \returns    An integer timer number for use as an argument to other
 *              timer functions.
 */

unsigned char   timer_allocate(void)
{
    /* Allocate 16 bit timers 1 and 3 before 8 bit timers 2 and 4 */
    if ( ! Timer_allocated[0] )
    {
	Timer_allocated[0] = 1;
	return 1;
    }
    else if ( ! Timer_allocated[2] )
    {
	Timer_allocated[2] = 1;
	return 3;
    }
    else if ( ! Timer_allocated[1] )
    {
	Timer_allocated[1] = 1;
	return 2;
    }
    else if ( ! Timer_allocated[3] )
    {
	Timer_allocated[3] = 1;
	return 4;
    }
    return 0;
}

/**
 *  Free a timer that was previously allocated with timer_allocate().
 *  This will make it available to the next timer_allocate() call.
 *  This does not
 *  
 *  \param  timer   Integer from 1 to 4 inclusive, specifying which timer
 *                  to free.
 */

void    timer_free(unsigned char timer)

{
    timer_stop(timer);
    Timer_allocated[timer-1] = 0;
}

/** @} */

