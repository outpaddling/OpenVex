 

#ifndef __platform_h__
#define __platform_h__

#ifndef DEBUG
#define DEBUG   1
#endif

#if defined(SDCC)   /* SDCC specifics */

#include <pic18fregs.h>
#include <delay.h>

#define NAKED_INTERRUPT_VECTOR(l)  __naked __interrupt l
#define C_LABEL(l)          _##l
#define INTERRUPT           __interrupt
#define NAKED_INTERRUPT     __naked __interrupt
#define FORMAT_CAST
#define DATA                __data
/* Make sure this matches compiler and linker script settings! */
#define STACK_BASE          (long DATA *)0x200
#define ADC_MASK            0x00
#define _ASM                __asm
#define _ENDASM             __endasm

#else   /* MCC18 specifics */

#include <p18f8520.h>
#include <delays.h>

/* FIXME: Multipliers in the argument limit the range */
#define delay1tcy(t)    Delay1TCYx(t)
#define delay10tcy(t)   Delay10TCYx(t)
#define delay100tcy(t)  Delay100TCYx(t)
#define delay10ktcy(t)  Delay10KTCYx(t)
#define delay100ktcy(t) Delay10KTCYx(t*10)
#define delay1mtcy(t)   Delay10KTCYx(t*100)

void InterruptVectorHigh(void);
void InterruptHandlerHigh(void);
#define NAKED_INTERRUPT_VECTOR(l)
#define C_LABEL(l)          l
#define INTERRUPT
#define NAKED_INTERRUPT
#define FORMAT_CAST (MEM_MODEL rom char*)
#define DATA
/* Make sure this matches compiler and linker script settings! */
#define STACK_BASE          (long DATA *)0x600

/* See ADC_V2 ADC_#ANAs in adc.h and pconfig.h */
#ifdef USE_OR_MASKS
#define ADC_MASK    0x00
#else
#define ADC_MASK    0xF0
#endif

#define _ASM        _asm
#define _ENDASM     _endasm

#endif

/* SDCC or MCC18 */
#define TOTAL_IO_PORTS          16
#define TOTAL_INTERRUPT_PORTS   6
#define TOTAL_MOTOR_PORTS       8
#define TOTAL_RC_CHANNELS       6
#define TOTAL_PWM_PORTS         8

/**
 * \addtogroup debug
 *  @{
 */


/** @} */

/*
 *  Simulator vs. Vex
 */
#ifdef  _SIMULATOR
#define RESET_VECTOR        0x000
#define HIGH_INT_VECTOR     0x008
#define LOW_INT_VECTOR      0x018
#else
#define RESET_VECTOR        0x800
#define HIGH_INT_VECTOR     0x808
#define LOW_INT_VECTOR      0x818
#endif

#endif

