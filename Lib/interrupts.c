/**************************************************************************
* Description: 
*   This file contains code for using the VEX interrupt ports.
*
*   Ports 1 and 2 each have a dedicated interrupt line and can be
*   configured to trigger on leading or trailing edges.
*
*   Ports 3 - 6 share the RB interrupt line, and are triggered by
*   any change to any of bits 4-7 on PORTB.
*
*   Something in this file must be referenced in order to link
*   InterruptHandlerLow() into the program.  controller_init()
*   References to Timer0_overflows will accomplish this.
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
***************************************************************************/

#include <stdio.h>
#include "platform.h"
#include "shaft_encoder.h"
#include "timer.h"
#include "sonar.h"
#include "interrupts.h"
#include "io.h"

/* Timer interrupt (overflow) counts.  Extend each timer to 32 bits */
unsigned int    Timer0_overflows;
unsigned int    Timer1_overflows;
unsigned long   Timer2_overflows;
unsigned int    Timer3_overflows;
unsigned long   Timer4_overflows;

/* Access via macros defined in interrupts.h */
unsigned char   Interrupt_port_in_use[6] = {0,0,0,0,0,0};

/* Sonar globals */
unsigned char           Sonar_on_iport[6] = {0,0,0,0,0,0};
unsigned char           Sonar_output_port[6];
volatile unsigned char  Sonar_data_available[6] = {0,0,0,0,0,0};
volatile unsigned short Sonar_echo_time[6] = {0,0,0,0,0,0};
volatile unsigned short Timer0_at_emit[6];
volatile unsigned short Timer0_at_echo[6]; 
extern unsigned char    Analog_ports;

/* Shaft encoder globals */
volatile unsigned int   Encoder_ticks[6] = {0,0,0,0,0,0};
unsigned char           Encoder_on_iport[6] = {0,0,0,0,0,0};
unsigned char           Quad_input_port[6] = {0,0,0,0,0,0};
extern unsigned char    Analog_ports;

/* Samples of digital inputs taken by ISR for good timing */
volatile unsigned char  Porta_sample, Portf_sample, Porth_sample;


/****************************************************************************
 * Description: 
 *  Select rising or trailing edge for the given Vex interrupt port.
 *  This only applies to ports 1 and 2, which have dedicated interrupt
 *  channels.  Ports 3 - 6 use shared RB interrupts, which always interrupt
 *  on both edges.  Hence, this function takes no action if passed
 *  a port value from 3 to 6.
 *
 *  While we cannot prevent the RB ISR from being triggered for on edges,
 *  we can code the ISR not to do anything for one edge or the other by simply
 *  checking the appropriate RB bit.
 *  E.g., if the bit is 0, it must have been a falling edge that triggered
 *  the interrupt.
 *  
 * History:
 *  Dec 2008     J Bacon
 ***************************************************************************/

void    interrupt_set_edge(unsigned char port,unsigned char mask)

{
    switch(port)
    {
	case    1:
	    INTCON2bits.INTEDG2 = mask;
	    break;
	case    2:
	    INTCON2bits.INTEDG3 = mask;
	    break;
	/*
	 *  Ports 3 - 6 use RB interrupts, which always trigger an
	 *  interrupt on both edges.  For those, we simply need to
	 *  look at the RB bits after the fact to see which edge it was.
	 */
    }
}


/****************************************************************************
 * Description: 
 *  Enable interrupts on the given port.  When interrupts are enabled,
 *  a transition on the given port will cause a branch to the low or
 *  high interrupt vector (selectable for each port).  Note that
 *  interrupt flags are set whether or not interrupts are enabled.
 *  This function and interrupt_disable() only determine whether a
 *  branch to the interrupt vector will occur.
 *
 * History:
 *  Dec 2008     J Bacon
 ***************************************************************************/

void    interrupt_enable(unsigned char port)

{
    switch(port)
    {
	case    1:
	    INTCON3bits.INT2IE = 1;
	    break;
	case    2:
	    INTCON3bits.INT3IE = 1;
	    break;
	case    3:
	case    4:
	case    5:
	case    6:
	    INTCONbits.RBIE = 1;
	    break;
    }
}


/****************************************************************************
 * Description: 
 *  Disable interrupts on the given port.  When interrupts are enabled,
 *  a transition on the given port will cause a branch to the low or
 *  high interrupt vector (selectable for each port).  Note that
 *  interrupt flags are set whether or not interrupts are enabled.
 *  This function and interrupt_enable() only determine whether a
 *  branch to the interrupt vector will occur.
 *
 * History:
 *  Dec 2008     J Bacon
 ***************************************************************************/

void    interrupt_disable(unsigned char port)

{
    switch(port)
    {
	case    1:
	    INTCON3bits.INT2IE = 0;
	    break;
	case    2:
	    INTCON3bits.INT3IE = 0;
	    break;
	case    3:
	case    4:
	case    5:
	case    6:
	    INTCONbits.RBIE = 0;
	    break;
    }
}


/****************************************************************************
 * Description: 
 *  The PIC processor has two interrupt vectors: one for low priority
 *  and one for high.  Each interrupt is configurable as either low or
 *  high.  This routine is the vector for all interrupts configured
 *  as low priority.
 *
 *  Since it is possible that multiple such interrupts could occur at
 *  virtually the same time, this routine checks all relevant flags
 *  before returning.
 *
 * History:
 *  Dec 2008     J Bacon
 ***************************************************************************/

/* 
 *  Unfortunately, #pragmas cannot be wrapped in SDCC.  The preprocessor
 *  will not do any substitutions in a #pragma line.
 */
#ifdef SDCC
#pragma code InterruptVectorLow 0x818
#else
#pragma code InterruptVectorLow=LOW_INT_VECTOR
#endif

void InterruptVectorLow(void) NAKED_INTERRUPT_VECTOR(2)
{
    _ASM
    goto C_LABEL(InterruptHandlerLow)
    _ENDASM;
}

#ifndef SDCC
#pragma code
#pragma interruptlow InterruptHandlerLow
#endif

void    InterruptHandlerLow(void) INTERRUPT
{
    int                     mask,
			    p;
    unsigned char           new_portb;
    
    /*
     *  Since the Vex interrupt ports 3 - 6 all use the RB interrupt,
     *  it is possible (though unlikely) that 2 to 4 sensors could generate
     *  interrupts within the time it takes the RB ISR to execute.
     *  Hence, the RB ISR code should be reentrant (able to be called again
     *  to process another interrupt, while it is already running, and
     *  without affecting the state of the current call).
     *
     *  The old_portb variable is used to store the value of PORTB
     *  until the next call to this ISR.  As such, it must be a static
     *  variable.  However, to make this code reentrant, each call
     *  to this function must not alter the effects of other calls on
     *  old_portb.
     *
     *  Normally, reentrant functions use only "auto" variables, which are
     *  automatically instantiated (allocated new space) every time the
     *  function is called.  Unfortunately, an auto variable does not
     *  retain its value between calls, so it cannot be used for old_portb.
     *
     *  To make the RB interrupt routine reentrant, we must ensure that
     *  each reentrant call to this ISR will only affect the bit in
     *  old_portb corresponding to the bit that triggered the interrupt.
     *  If this ISR is itself interrupted by another RB interrupt, the
     *  newer call will will always have been triggered by a different
     *  PORTB bit.  In order to trigger an interrupt on a bit that has
     *  not been completely processed by this ISR, one of the 4 sensors on
     *  ports 3 - 6 would have to generate 2 interrupts in the time it takes
     *  to run the RB ISR 4 times.  None of the Vex sensors will generate
     *  interrupts that fast, so we don't need to handle this case.
     *
     *  The volatile modifier tells the compiler that the value of this
     *  variable could be changed at any instant by forces outside this
     *  thread (in this case, another interrupt could execute a second
     *  thread of this ISR unbeknownst to this thread).  The compiler
     *  will then avoid certain optimizations that assume that the
     *  variable contents are under the complete control of this thread.
     */

    /*
     *  Ideally we should know the initial state of PORTB in order
     *  to prevent erroneously calling some handlers once.  In practice,
     *  this isn't feasible, since it could change many times before
     *  interrupts are enabled.  Hence, we just initialize old_portb[]
     *  to 0s.
     */
    volatile static unsigned char    old_portb = 0;
    
    /*
     *  INTERRUPT_IN[3-6] (RB4, RB5, RB6, or RB7) all use INTRB.
     *  RBIF is set by any change to any of these bits.  We check
     *  RBIF first, since another RB interrupt could occur at any time,
     *  including while in the ISR.  The sooner we process this interrupt
     *  the less likely this is to happen.  This is not an issue for
     *  the dedicated interrupts (INTERRUPT_IN[1-2]), since no individual
     *  VEX sensor can generate interrupts faster than the ISR can
     *  process them.
     */
    
    /*
     *  Sample digital inputs immediately for quad encoders.  It's cheaper
     *  to sample them all than to figure out which ones need to be
     *  sampled, if any.
     */
    Porta_sample = PORTA;
    Portf_sample = PORTF;
    Porth_sample = PORTH;

    if (INTCONbits.RBIF)
    {
	/*
	 *  Must read or write PORTB and then clear the interrupt flag
	 *  to clear the interrupt condition.
	 */
	new_portb = PORTB;
	INTCONbits.RBIF = 0;
	
	/*
	 *  Do as I say, not as I do...
	 *  I'm not crazy about having loop overhead in an ISR.
	 *  The execution path through an ISR should be as short
	 *  as possible to minimize the chance of the ISR itself being
	 *  interrupted.  But, it's just a toy robot, and the rare
	 *  hiccup isn't going to kill anyone.  When 4 ports share the
	 *  same interrupt flag, you can't do much better than this.
	 *
	 *  Multiple PORTB bits could trigger an RB interrupt at virtually
	 *  the same time, so we check all 4 flags and process all
	 *  pending inputs while we're here.
	 *
	 *  Interrupt port 3 = PORTB bit 4 ...
	 */
	 
	for (p = 3, mask = 0x10; p <= 6; ++p, mask <<= 1)
	{
	    /* Has the PORTB bit for port p changed? */
	    if ( (new_portb ^ old_portb) & mask )
	    {
		/* Rising edge on PORTB bit "port"? */
		if ( new_portb & mask ) /* Rising edge */
		{
		    if ( ENCODER_ON_IPORT(p) == ENCODER_STD )
			SHAFT_ENCODER_ISR(p);
		    else if ( ENCODER_ON_IPORT(p) == ENCODER_QUAD )
			quad_encoder_isr(p);
		    else if ( SONAR_ON_IPORT(p) )
			sonar_emit_isr(p);
		    old_portb |= mask;      /* Update "old" bit state */
		}
		/* Falling edge on PORTB bit "port"? */
		else
		{
		    /* 
		     *  Ignore falling edge for shaft encoder to avoid
		     *  counting twice as many pulses as on interrupts
		     *  1 & 2, which only trigger on one edge.
		     */
		    if ( SONAR_ON_IPORT(p) )
			sonar_echo_isr(p);
		    old_portb &= ~mask;     /* Update "old" bit state */
		}
	    }
	}
    }
    
    /*
     *  INTERRUPT_IN1 uses INT2/RB2.  There can be only one device generating
     *  these interrupts.
     */
    if (INTCON3bits.INT2IF)
    {
	INTCON3bits.INT2IF = 0;
	if ( ENCODER_ON_IPORT(1) == ENCODER_STD )
	    SHAFT_ENCODER_ISR(1);
	else if ( ENCODER_ON_IPORT(1) == ENCODER_QUAD )
	    quad_encoder_isr(1);
	else if ( SONAR_ON_IPORT(1) )
	{
	    if (INTCON2bits.INTEDG2)    /* Rising edge? */
		sonar_emit_isr(1);
	    else
		sonar_echo_isr(1);
	}
    }
    
    /*
     *  INTERRUPT_IN2 uses INT3/RB3.  There can be only one device generating
     *  these interrupts.
     */
    if (INTCON3bits.INT3IF)
    {
	INTCON3bits.INT3IF = 0;
	if ( ENCODER_ON_IPORT(2) == ENCODER_STD )
	    SHAFT_ENCODER_ISR(2);
	else if ( ENCODER_ON_IPORT(2) == ENCODER_QUAD )
	    quad_encoder_isr(2);
	else if ( SONAR_ON_IPORT(2) )
	{
	    if (INTCON2bits.INTEDG3)    /* Rising edge? */
		sonar_emit_isr(2);
	    else
		sonar_echo_isr(2);
	}
    }
    
    /* Timer 0 overflow interrupt */
    if ( INTCONbits.T0IF )
    {
	INTCONbits.T0IF = 0;
	++Timer0_overflows;
    }

    /* Timer 1 overflow interrupt */
    if ( PIR1bits.TMR1IF )
    {
	PIR1bits.TMR1IF = 0;
	++Timer1_overflows;
    }
    
    /* Timer 2 overflow interrupt */
    /* Timer 2 match to PR2 (timer 2 period register) interrupt */
    if ( PIR1bits.TMR2IF )
    {
	PIR1bits.TMR2IF = 0;
	++Timer2_overflows;
	/*
	 *  Timer2_overflows should hold 24 bits to extend the 8-bit timer
	 *  but is defined as a long.
	 */
	Timer2_overflows &= 0x00ffffff;
    }
    
    /* Timer 3 overflow interrupt */
    if ( PIR2bits.TMR3IF )
    {
	PIR2bits.TMR3IF = 0;
	++Timer3_overflows;
    }
    
    /* Timer 4 overflow interrupt */
    if ( PIR3bits.TMR4IF )
    {
	PIR3bits.TMR4IF = 0;
	++Timer4_overflows;
	/* Timer4_overflows should hold 24 bits to extend the 8-bit timer
	 *  but is defined as a long.
	 */
	Timer4_overflows &= 0x00ffffff;
    }
}


/************************************************************************
 *  Interrupt service routine for ultrasonic sensor.  This sensor
 *  transitions its digital output from 0 to 1 when the ultrasound pulse
 *  is emitted, and from 1 to 0 when an echo is received, or after
 *  a built-in timeout.  Hence, rising edge and falling edge interrupts
 *  must both be handled, and the time difference between them indicates
 *  the distance of the reflecting object.
 *
 * History:
 *  Dec 2008     J Bacon
 ***********************************************************************/

void sonar_emit_isr(unsigned char interrupt_port) 

{
    /* Record time of pulse start */
    TIMER0_READ16(Timer0_at_emit[interrupt_port-1]);
    
    switch(interrupt_port)
    {
	case    1:
	    /* Catch next falling edge when echo returns */
	    INTCON2bits.INTEDG2 = 0;
	    break;

	case    2:
	    /* Catch next falling edge when echo returns */
	    INTCON2bits.INTEDG3 = 0;
	    break;

	/* Shared interrupts ports always trigger on both edges,
	   so there's nothing to do here. */
	case    3:
	case    4:
	case    5:
	case    6:
	    break;
    }
}


void sonar_echo_isr(unsigned char interrupt_port) 

{
    TIMER0_READ16(Timer0_at_echo[interrupt_port-1]);
    
    SET_SONAR_ECHO_TIME(interrupt_port,
	(Timer0_at_echo[interrupt_port-1] - 
	Timer0_at_emit[interrupt_port-1]));
    SET_SONAR_DATA_AVAILABLE(interrupt_port);
}


/****************************************************************************
 *  Process quadrature encoder interrupt on any of the 6 interrupt ports.
 *  The second line from the quadrature encoder outputs the same
 *  signal, but out of phase with the first.  As a result, the
 *  state of one signal when the other rises or falls indicates
 *  direction of rotation.
 *
 *  __--__--__--__--
 *   __--__--__--__--
 *
 * History:
 *  May 2009    J Bacon
 ***************************************************************************/

void    quad_encoder_isr(unsigned char interrupt_port)

{
    unsigned char   sample = 0;
    
    /*
     *  Use a sample of the digital input port taken at the start of the
     *  ISR.  It's possible, though unlikely, that at high RPMs
     *  and with other interrupts being serviced, enough
     *  time could pass between the interrupt trigger and arriving in
     *  this function for the state of the secondary encoder signal
     *  to change.  Sampling PORTF in this function and comparing to
     *  Portf_sample confirmed that this happens on rare occasions.
     *  (with quad encoder on input port 8, PORTF & 0x04 read 0
     *  while Portf_sample & 0x04 read 1)
     */ 
    switch(Quad_input_port[interrupt_port-1])
    {
	case    1:
	    sample = Porta_sample & 0x01;
	    break;
	case    2:
	    sample = Porta_sample & 0x02;
	    break;
	case    3:
	    sample = Porta_sample & 0x04;
	    break;
	case    4:
	    sample = Porta_sample & 0x08;
	    break;
	case    5:
	    sample = Porta_sample & 0x20;
	    break;
	case    6:
	    sample = Portf_sample & 0x01;
	    break;
	case    7:
	    sample = Portf_sample & 0x02;
	    break;
	case    8:
	    sample = Portf_sample & 0x04;
	    break;
	case    9:
	    sample = Portf_sample & 0x08;
	    break;
	case    10:
	    sample = Portf_sample & 0x10;
	    break;
	case    11:
	    sample = Portf_sample & 0x01;
	    break;
	case    12:
	    sample = Portf_sample & 0x02;
	    break;
	case    13:
	    sample = Porth_sample & 0x10;
	    break;
	case    14:
	    sample = Porth_sample & 0x20;
	    break;
	case    15:
	    sample = Porth_sample & 0x40;
	    break;
	case    16:
	    sample = Porth_sample & 0x80;
	    break;
	default:
	    break;
    }
    
    /*
     *  Filter out jitter. Only count rotations in the same direction as
     *  the previous.
     */
    if ( sample == 0 )
	--Encoder_ticks[interrupt_port-1];
    else
	++Encoder_ticks[interrupt_port-1];
}

