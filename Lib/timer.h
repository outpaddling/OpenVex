/**************************************************************************
* Description: 
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
*       Created Dec 2008    Jason Bacon
***************************************************************************/

#ifndef __timer_h__
#define __timer_h__

#define TIMER0_PRESCALE_MASK_2    0x00
#define TIMER0_PRESCALE_MASK_4    0x01
#define TIMER0_PRESCALE_MASK_8    0x02
#define TIMER0_PRESCALE_MASK_16   0x03
#define TIMER0_PRESCALE_MASK_32   0x04
#define TIMER0_PRESCALE_MASK_64   0x05
#define TIMER0_PRESCALE_MASK_128  0x06
#define TIMER0_PRESCALE_MASK_256  0x07
#define TIMER0_PRESCALE_MASK_OFF  0x08

/*
 *  Constants like this are just to improve readability.  Yeah, it's trivial,
 *  but we would have to look closely at the code to realize what the 1000 is
 *  doing there, whereas MS_PER_SEC makes it obvious at a glance.
 */
#define MS_PER_SEC          1000

/*
 *  Implement reads as macros to avoid inaccurate results.  A function
 *  call can take several timer ticks, and MCC does not support
 *  inline functions.
 *
 *  Furthermore, since the C language does not define order of evaluation
 *  for an expression such as TMR0L | ((unsigned short)TMR0H << 8),
 *  and TMR0L must be read first (see spec sheet), reading MUST be done
 *  in two separate statements to guarantee that TMR0L is read first.
 */

/**
 *  \addtogroup timer_low
 *  @{
 */

/**
 *  Read the low byte of timer 0, placing the value in v and returning it.  The
 *  variable v should be an unsigned char for best results.
 */
 
#define TIMER0_READ8(v)             ((v) = (unsigned char)TMR0L)

/**
 *  Read both bytes of the Timer0 register, both returning the value and assigning
 *  it to v, which must be a 16 bit integer variable.
 *  
 *  Note that TMR0L is the low byte of the Timer0 register, but TMR0H is NOT
 *  the high byte. Reading TMR0L causes the processor to place a
 *  snapshot of the actual Timer0 high byte in TMR0H.  This ensures that both
 *  bytes are sampled during the same clock cycle, and the 16
 *  bit value taken from TMR0L and TMR0H is coherent.  It also means that
 *  TMR0L must be read first.
 *  
 *  Note that the 16 bit timer value cannot be read using a simple
 *  expression like (TMR0L + (unsigned short)TMR0H << 8), since the C 
 *  standard does not specify which operand of '+' is evaluated first.
 *  The ',' operator, on the other hand, guarantees that operands are
 *  evaluated left to right.
 *  It also guarantees that the value of the overall expression is the
 *  value of the last operand.  By assigning the 16-bit value
 *  to v in the last operand of the ',' operator, this macro is able to return the
 *  full 16 bit timer value.
 *
 *  The value returned will be the timer value during execution of the first
 *  instruction of TIMER0_READ16().  The macro itself will consume several timer
 *  cycles reading TMR0L, TMR0H, and performing the | operation.
 */

#define TIMER0_READ16(v)    ((v) = TMR0L, (v) |= (unsigned short)TMR0H << 8)

/**
 *  Return the number of milliseconds passed since the last timer 0
 *  reset.  See timer0_read32() for accuracy and period information.
 */

#define TIMER0_ELAPSED_MS   ( timer0_read32() / TIMER0_TICKS_PER_MS )
#define TIMER0_ELAPSED_SECONDS  ( TIMER0_ELAPSED_MS / MS_PER_SEC )

/**
 *  Reports the current timer 0 prescale factor.  If prescaling is disabled,
 *  this value is 1.  Otherwise, it is a power of 2 from 2 to 256, inclusive.
 */

#define TIMER0_PRESCALE     ( (T0CON & 0x08) ? 1 : 2 << (T0CON & 0x07) )

/**
 *  Reports the number of timer 0 ticks per millisecond, which is
 *  10,000 / the current prescale factor.  ( The raw internal timer clock is
 *  Fosc/4 = 10 MHz. )
 *
 *  Note that
 *  for prescale factors > 16, integer truncation occurs, since 10,000
 *  is not divisible by powers of 2 greater than 16.  Hence, the value
 *  returned by this macro should not be used in a cumulative fashion
 *  if prescaling > 16 is in effect.
 */

#define TIMER0_TICKS_PER_MS ( 10000 / TIMER0_PRESCALE )

#define TIMER0_RESET()      { T0CON = 0; Timer0_overflows = 0; }

/**
 *  Write the value of Timer0.  Both 8 and 16 bit writes can be performed in
 *  either 8 or 16 bit mode.  ( The mode only determines whether the
 *  timer overflows and writes T0IF at 0xFF or 0xFFFF. )
 */

/*
 * History:
 *  Dec 2008    J Bacon
 */

/*
 *  TMR0H is not the high byte of the timer register, but a buffer
 *  to and from it.  It should be written first, and when TMR0L is
 *  written, both bytes will be transferred to the timer0 register
 *  at the same time.
 */

#define TIMER0_WRITE16(v) \
{ \
    TMR0H = ((unsigned short)(v) >> 8) & 0xff; \
    TMR0L = ((unsigned short)(v)) & 0xff; \
}


/**
 *  Write the value of Timer0.  Both 8 and 16 bit writes can be performed in
 *  either 8 or 16 bit mode.  ( The mode only determines whether the
 *  timer overflows and writes T0IF at 0xFF or 0xFFFF. )
 */

/*
 * History:
 *  Dec 2008    J Bacon
 */

#define TIMER0_WRITE8(v)  (TMR0L = (v))


/**
 *  Select a prescale value for Timer0.  The prescaler slows the timer
 *  increments by a given factor.  Without prescaling, the timer is
 *  incremented at Fosc/4, or 10MHz.  ( Every 1/10 microsecond. )
 *  
 *  \htlmonly
 *  <table border=0 cellpadding=0>
 *  <tr>
 *  <td>Prescale    <td>Period \n
 *  <tr>
 *  <td>TIMER0_PRESCALE_MASK_2           <td>0.2 us \n
 *  <tr>
 *  <td>TIMER0_PRESCALE_MASK_4           <td>0.4 us \n
 *  <tr>
 *  <td>TIMER0_PRESCALE_MASK_8           <td>0.8 us \n
 *  <tr>
 *  <td>TIMER0_PRESCALE_MASK_16          <td>1.6 us      * Maximum for precise ms timing (625 ticks = 1ms)\n
 *  <tr>
 *  <td>TIMER0_PRESCALE_MASK_32          <td>3.2 us \n
 *  <tr>
 *  <td>TIMER0_PRESCALE_MASK_64          <td>6.4 us \n
 *  <tr>
 *  <td>TIMER0_PRESCALE_MASK_128         <td>12.8 us \n
 *  <tr>
 *  <td>TIMER0_PRESCALE_MASK_256         <td>25.6 us \n
 *  </table>
 *  \endhtmlonly
 */

/*
 * History:
 *  Dec 2008    J Bacon
 */

#define TIMER0_SET_PRESCALE(mask)   (T0CON = (T0CON & 0xf0) | (mask))


/**
 *  Clear the interrupt flag for Timer0.  The flag is set when Timer0
 *  overflows (whether or not interrupts are enabled).
 */

/*
 * History: 
 *  Dec 2008    J Bacon
 */

#define TIMER0_CLEAR_INTERRUPT_FLAG()   { INTCONbits.T0IF = 0; }


/**
 *  Start Timer0.  ( Allow Timer0 registers to increment. )
 */

/*
 * History:
 *  Dec 2008    J Bacon 
 */

//T0CONbits.TMR0ON = 1;
#define TIMER0_START()  { T0CON |= 0X80; }


/**
 *  Stop Timer0.  ( Disable incrementing of Timer0 registers. )
 */

/*
 * History:
 *  Dec 2008    J Bacon 
 */

//T0CONbits.TMR0ON = 0;
#define TIMER0_STOP()   { T0CON &= ~0x80; }

/**
 *  Enable interrupts for Timer0.  An interrupt will be signalled and a
 *  branch to the appropriate interrupt vector will occur when the timer
 *  overflows from 0xFF or 0xFFFF to 0, depending on whether in 8 or 16
 *  bit mode.
 */

/*
 * History:
 *  Dec 2008    J Bacon
 */

#define TIMER0_ENABLE_INTERRUPTS() \
{ \
    INTCON2bits.T0IP = 0; \
    INTCONbits.T0IE = 1;    /* timer0 interrupts */ \
    INTCONbits.PEIE = 1;    /* Peripheral interrupts */ \
    INTCONbits.GIE = 1;     /* General interrupts */ \
}


/**
 *  Disable interrupts for Timer0.  Note that the interrupt flag will
 *  continue to be set when Timer0 overflows.  However, a branch to the
 *  interrupt vector will not occur.
 */

/*
 * History:
 *  Dec 2008    J Bacon
 */

/* Leaves peripheral and general interrupts on */
#define TIMER0_DISABLE_INTERRUPTS() { INTCONbits.T0IE = 0; }


/**
 *  Set Timer0 to operate in 16 bit mode.
 */

/*
 * History:
 *  Dec 2008    J Bacon
 */

//T0CONbits.T08BIT = 0;
#define TIMER0_SET_WIDTH_16() { T0CON &= ~0x40; }


/**
 *  Set Timer0 to operate in 16 bit mode.
 */

//T0CONbits.T08BIT = 1;
#define TIMER0_SET_WIDTH_8()  { T0CON |= 0x40; }

/*
 *  Timer 1
 */

#define TIMER1_READ8(v)     ((v) = TMR1L)
#define TIMER1_READ16(v)    ((v) = TMR1L, (v) |= (unsigned short)TMR1H << 8)
#define TIMER1_WRITE8(v)    (TMR1L = (v))
#define TIMER1_WRITE16(v) \
{ \
    TMR1H = ((unsigned short)(v) >> 8) & 0xff; \
    TMR1L = (v) & 0xff; \
}

#define TIMER1_START()  { T1CONbits.TMR1ON = 1; }
#define TIMER1_STOP()   { T1CONbits.TMR1ON = 0; }

#define TIMER1_SET_WIDTH_16()   { T1CONbits.RD16 = 1; }
#define TIMER1_SET_WIDTH_8()    { T1CONbits.RD16 = 0; }

#define TIMER1_PRESCALE         ( 1 << ((T1CON & 0X30) >> 4) )

#define TIMER1_PRESCALE_MASK_1   0x00
#define TIMER1_PRESCALE_MASK_2   0x10
#define TIMER1_PRESCALE_MASK_4   0x20
#define TIMER1_PRESCALE_MASK_8   0x30

#define TIMER1_SET_PRESCALE(mask)   { T1CON = (T1CON & 0xcf) | (mask); }

//#define TIMER1_POSTSCALE
//#define TIMER1_SET_POSTSCALE
//#define TIMER1_PERIOD
//#define TIMER1_SET_PERIOD

#define TIMER1_TICKS_PER_MS ( 10000 / TIMER1_PRESCALE )
#define TIMER1_ELAPSED_MS   ( timer1_read32() / TIMER1_TICKS_PER_MS )

#define TIMER1_CLEAR_INTERRUPT_FLAG()   { PIR1bits.TMR1IF = 0; }
#define TIMER1_ENABLE_INTERRUPTS()      { PIE1bits.TMR1IE = 1; }
#define TIMER1_DISABLE_INTERRUPTS()     { PIE1bits.TMR1IE = 0; }

/*
 *  Timer 2
 */

#define TIMER2_READ8(v)     ((v) = TMR2)
#define TIMER2_READ16(v)    ((v) = TMR2, \
	    (v) |= (Timer2_overflows & 0xff))
#define TIMER2_WRITE8(v)    (TMR2 = (v))
#define TIMER2_WRITE16(v) \
{ \
    Timer2_overflows = (Timer2_overflows & 0xffffff00) | ((v) & 0xff00); \
    TMR2 = (v) & 0xff; \
}

#define TIMER2_WRITE_PR(v)  (PR2 = (v))

#define TIMER2_START()  { T2CONbits.TMR2ON = 1; }
#define TIMER2_STOP()   { T2CONbits.TMR2ON = 0; }

//#define TIMER2_SET_WIDTH_16()   { T1CONbits.RD16 = 1; }
//#define TIMER2_SET_WIDTH_8()    { T1CONbits.RD16 = 0; }

/*
 *  2 ^ (T2CKPS1:T2CKPS0 * 2) 
 *  This code does not work if PS1:PS0 == 11. Use the masks!
 */
#define TIMER2_PRESCALE         ( 1 << ((T2CON & 0X03) << 1) )

#define TIMER2_PRESCALE_MASK_1   0x00
#define TIMER2_PRESCALE_MASK_4   0x01
#define TIMER2_PRESCALE_MASK_16  0x02

#define TIMER2_SET_PRESCALE(mask)   { T2CON = (T2CON & 0xfc) | (mask); }

//#define TIMER2_POSTSCALE
//#define TIMER2_SET_POSTSCALE
//#define TIMER2_PERIOD
//#define TIMER2_SET_PERIOD

#define TIMER2_TICKS_PER_MS ( 10000 / TIMER2_PRESCALE )
#define TIMER2_ELAPSED_MS   ( timer2_read32() / TIMER2_TICKS_PER_MS )

#define TIMER2_CLEAR_INTERRUPT_FLAG()   { PIR1bits.TMR2IF = 0; }
#define TIMER2_ENABLE_INTERRUPTS()      { PIE1bits.TMR2IE = 1; }
#define TIMER2_DISABLE_INTERRUPTS()     { PIE1bits.TMR2IE = 0; }

/*
 *  Timer 3
 */

#define TIMER3_READ8(v)     ((v) = TMR3L)
#define TIMER3_READ16(v)    ((v) = TMR3L, (v) |= (unsigned short)TMR3H << 8)
#define TIMER3_WRITE8(v)    (TMR3L = (v))
#define TIMER3_WRITE16(v) \
{ \
    TMR3H = ((unsigned short)(v) >> 8) & 0xff; \
    TMR3L = (v) & 0xff; \
}

#define TIMER3_START()  { T3CONbits.TMR3ON = 1; }
#define TIMER3_STOP()   { T3CONbits.TMR3ON = 0; }

#define TIMER3_SET_WIDTH_16()   { T3CONbits.RD16 = 1; }
#define TIMER3_SET_WIDTH_8()    { T3CONbits.RD16 = 0; }

#define TIMER3_PRESCALE         ( 1 << ((T3CON & 0X30) >> 4) )

#define TIMER3_PRESCALE_MASK_1   0x00
#define TIMER3_PRESCALE_MASK_2   0x10
#define TIMER3_PRESCALE_MASK_4   0x20
#define TIMER3_PRESCALE_MASK_8   0x30

#define TIMER3_SET_PRESCALE(mask)   { T3CON = (T3CON & 0xcf) | (mask); }

//#define TIMER3_POSTSCALE
//#define TIMER3_SET_POSTSCALE
//#define TIMER3_PERIOD
//#define TIMER3_SET_PERIOD

#define TIMER3_TICKS_PER_MS ( 10000 / TIMER3_PRESCALE )
#define TIMER3_ELAPSED_MS   ( timer3_read32() / TIMER3_TICKS_PER_MS )

#define TIMER3_CLEAR_INTERRUPT_FLAG()   { PIR2bits.TMR3IF = 0; }
#define TIMER3_ENABLE_INTERRUPTS()      { PIE2bits.TMR3IE = 1; }
#define TIMER3_DISABLE_INTERRUPTS()     { PIE2bits.TMR3IE = 0; }

/*
 *  Timer 4
 */

// Work around bug in pic18f8520.h
#ifdef SDCC
#define TMR4ON  TMR2ON
#endif

#define TIMER4_READ8(v)     ((v) = TMR4)
#define TIMER4_READ16(v)    ((v) = TMR4, \
	    (v) |= (Timer4_overflows & 0xff))
#define TIMER4_WRITE8(v)    (TMR4 = (v))
#define TIMER4_WRITE16(v) \
{ \
    Timer4_overflows = (Timer4_overflows & 0xffffff00) | ((v) & 0xff00); \
    TMR4 = (v) & 0xff; \
}

#define TIMER4_WRITE_PR(v)  (PR4 = (v))

#define TIMER4_START()  { T4CONbits.TMR4ON = 1; }
#define TIMER4_STOP()   { T4CONbits.TMR4ON = 0; }

//#define TIMER4_SET_WIDTH_16()   { T1CONbits.RD16 = 1; }
//#define TIMER4_SET_WIDTH_8()    { T1CONbits.RD16 = 0; }

/*
 *  2 ^ (T4CKPS1:T4CKPS0 * 2) 
 *  This code does not work if PS1:PS0 == 11. Use the masks!
 */
#define TIMER4_PRESCALE         ( 1 << ((T4CON & 0X03) << 1) )

#define TIMER4_PRESCALE_MASK_1   0x00
#define TIMER4_PRESCALE_MASK_4   0x01
#define TIMER4_PRESCALE_MASK_16  0x02

#define TIMER4_SET_PRESCALE(mask)   { T4CON = (T4CON & 0xfc) | (mask); }

//#define TIMER4_POSTSCALE
//#define TIMER4_SET_POSTSCALE
//#define TIMER4_PERIOD
//#define TIMER4_SET_PERIOD

#define TIMER4_TICKS_PER_MS ( 10000 / TIMER4_PRESCALE )
#define TIMER4_ELAPSED_MS   ( timer4_read32() / TIMER4_TICKS_PER_MS )

#define TIMER4_CLEAR_INTERRUPT_FLAG()   { PIR3bits.TMR4IF = 0; }
#define TIMER4_ENABLE_INTERRUPTS()      { PIE3bits.TMR4IE = 1; }
#define TIMER4_DISABLE_INTERRUPTS()     { PIE3bits.TMR4IE = 0; }

/*
 *  System timer
 */

#define SYSTEM_TIMER_TICKS()    timer0_read32()
#define SYSTEM_TIMER_MS()       TIMER0_ELAPSED_MS
#define SYSTEM_TIMER_SECONDS()  (SYSTEM_TIMER_MS() / MS_PER_SEC)

/** @} */

#define TIMER0_INTERRUPT_FLAG       INTCONbits.TMR0IF

extern unsigned int    Timer0_overflows;
extern unsigned int    Timer1_overflows;
extern unsigned long   Timer2_overflows;
extern unsigned int    Timer3_overflows;
extern unsigned long   Timer4_overflows;

/* timer.c */
unsigned long timer0_read32(void);
unsigned long timer1_read32(void);
unsigned long timer2_read32(void);
unsigned long timer3_read32(void);
unsigned long timer4_read32(void);
void timer0_init(void);

/* timer_simple.c */
unsigned long timer_read_ms(unsigned char timer);
void timer_clear(unsigned char timer);
void timer_start(unsigned char timer);
void timer_stop(unsigned char timer);
unsigned char timer_allocate(void);
void timer_free(unsigned char timer);

#endif

