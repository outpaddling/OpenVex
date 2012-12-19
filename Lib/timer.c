/**************************************************************************
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
*
*   The PIC 18f8520 has 5 timers:
*       Timer0: 8/16 bit timer/counter
*       Timer1: 16 bit timer/counter
*       Timer2: 8 bit timer/counter
*       Timer3: 16 bit timer/counter
*       Timer4: 8 bit timer/counter
*
***************************************************************************
*   History:
*       Dec 2008    J Bacon
***************************************************************************/

/**
 *  \defgroup timer_low Low Level Timer Functions
 *  @{
 *
 *  These functions and macros provide direct access to the PIC 18f8520 timers,
 *  each of which has a different set of capabilities.
 *
 *  Note: Timer0 is reserved for use by OpenVex for sonar and delay
 *  functions, etc.
 */

#include <stdio.h>
#include "platform.h"
#include "vex_usart.h"
#include "io.h"
#include "timer.h"
#include "interrupts.h"
#include "sonar.h"

/* Timer interrupt (overflow) counts.  Extend each timer to 32 bits */
extern unsigned int    Timer0_overflows;
extern unsigned int    Timer1_overflows;
extern unsigned long   Timer2_overflows;
extern unsigned int    Timer3_overflows;
extern unsigned long   Timer4_overflows;

unsigned char   Timer_allocated[4];

/* This should be in pic18f8520.h, but for now... */
#if 0
typedef union {
    struct {
      unsigned T0PS0:1;
      unsigned T0PS1:1;
      unsigned T0PS2:1;
      unsigned PSA:1;
      unsigned T0SE:1;
      unsigned T0CS:1;
      unsigned T08BIT:1;
      unsigned TMR0ON:1;
    };
} __T0CONbits_t;

/* This should be in libdev/pic18f8520.c, but for now... */
/* volatile __T0CONbits_t __at (0xfd5) T0CONbits; */
__T0CONbits_t *T0CONbits_ptr = 0xfd5;
#define T0CONbits   (*T0CONbits_ptr)
#endif

#ifdef SDCC
#define T2OUTPS0    TOUTPS0
#define T2OUTPS1    TOUTPS1
#define T2OUTPS2    TOUTPS2
#define T2OUTPS3    TOUTPS3 
#endif

/****************************************************************************
 *  Timer0 functions
 ***************************************************************************/

/**
 *  Read the 32-bit value of Timer0, which includes a 16 bit
 *  sample from TMR0L and TMR0H, concatenated with the Timer0_overflow
 *  counter.  ( When Timer0 rolls over from
 *  0xffff to 0x0000, it generates an interrupt.  The ISR increments the
 *  16-bit Timer0_overflows, in essence maintaining a 32-bit Timer0 value. )
 *
 *  Before using this function, you must enable Timer0 interrupts
 *  with timer0_interrupts_on() and enable the timer with timer0_start().
 *  To make sense of the value returned requires knowing the prescale
 *  value, which is set with timer0_set_prescale() and read back with
 *  TIMER0_GET_PRESCALE or TIMER0_TICKS_PER_MS.
 *
 *  The value returned contains a lag of a few microseconds due to
 *  function calling and computational overhead.  In addition, if Timer0
 *  is sampled at the moment it rolls over from 0xffff to 0x0000, there
 *  will be an additional lag of roughly 1 microsecond due to resampling
 *  to get a coherent value.
 */

unsigned long   timer0_read32(void)

{
    unsigned short  high, low0, low1;
    
    TIMER0_READ16(low0);
    high = Timer0_overflows;
    TIMER0_READ16(low1);
    /*
     *  If a Timer0 overflow occurs during TIMER0_READ16(low0)
     *  or high = Timer0_overflows, then high may have been incremented
     *  by the ISR after sampling low0, but before we read it.  It's
     *  impossible to know for sure, since each statement entails
     *  several machine instructions, and the interrupt could occur
     *  after any one of them.  The only guaranteed way to produce
     *  a coherent 32-bit value is to resample Timer0_overflows to
     *  match low1.  This produces an additional lag of about 1 us.
     */
    if ( low1 < low0 )
	return low1 | ((unsigned long)Timer0_overflows << 16);
    else
	return low0 | ((unsigned long)high << 16);
}


unsigned long   timer1_read32(void)

{
    unsigned short  high, low0, low1;
    
    TIMER1_READ16(low0);
    high = Timer1_overflows;
    TIMER1_READ16(low1);
    /*
     *  If a Timer0 overflow occurs during TIMER0_READ16(low0)
     *  or high = Timer0_overflows, then high may have been incremented
     *  by the ISR after sampling low0, but before we read it.  It's
     *  impossible to know for sure, since each statement entails
     *  several machine instructions, and the interrupt could occur
     *  after any one of them.  The only guaranteed way to produce
     *  a coherent 32-bit value is to resample Timer0_overflows to
     *  match low1.  This produces an additional lag of about 1 us.
     */
    if ( low1 < low0 )
	return low1 | ((unsigned long)Timer1_overflows << 16);
    else
	return low0 | ((unsigned long)high << 16);
}


unsigned long   timer2_read32(void)

{
    unsigned short  low;

    TIMER2_READ8(low);
    return low | (Timer2_overflows << 8);
}


unsigned long   timer3_read32(void)

{
    unsigned short  high, low0, low1;
    
    TIMER3_READ16(low0);
    high = Timer3_overflows;
    TIMER3_READ16(low1);
    /*
     *  If a Timer0 overflow occurs during TIMER0_READ16(low0)
     *  or high = Timer0_overflows, then high may have been incremented
     *  by the ISR after sampling low0, but before we read it.  It's
     *  impossible to know for sure, since each statement entails
     *  several machine instructions, and the interrupt could occur
     *  after any one of them.  The only guaranteed way to produce
     *  a coherent 32-bit value is to resample Timer0_overflows to
     *  match low1.  This produces an additional lag of about 1 us.
     */
    if ( low1 < low0 )
	return low1 | ((unsigned long)Timer3_overflows << 16);
    else
	return low0 | ((unsigned long)high << 16);
}


unsigned long   timer4_read32(void)

{
    unsigned short  low;

    TIMER4_READ8(low);
    return low | (Timer4_overflows << 8);
}


/****************************************************************************
 *  Set up and enable Timer0 for use with ultrasonic range finders.
 *
 *  FIXME: Generalize this function and move it out of sonar.c
 *          It should serve other purposes besides the sonar.
 *
 * History:
 *  Dec 2008     J Bacon
 ***************************************************************************/

void    timer0_init(void)

{
    static char timer0_running = 0;
    
    if ( ! timer0_running )
    {
	/* Start with a clean slate */
	TIMER0_RESET();
	
	/*
	 *  Distant sonar echos may take a while, so an 8 bit counter
	 *  may be too limited.
	 */
	TIMER0_SET_WIDTH_16();
	
	/*
	 *  By trial and error, I found that a prescale of 16 is fast
	 *  enough to provide good resolution, and slow enough to
	 *  prevent too many overflows.
	 */
	TIMER0_SET_PRESCALE(TIMER0_PRESCALE_MASK_16);
	
	/*
	 *  Not strictly necessary, but we can extend the 16 bit counter
	 *  by counting the overflow interrupts.
	 */
	TIMER0_ENABLE_INTERRUPTS();
	
	TIMER0_CLEAR_INTERRUPT_FLAG();
	TIMER0_START();
	timer0_running = 1;
    }
}

/** @} */

