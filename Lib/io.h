/**************************************************************************
* Description: 
*   Macros, typedefs, and prototypes for dealing with the VEX I/O ports.
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
*       Created Dec 2008    Jason W. Bacon
***************************************************************************/

/**
 * \addtogroup IO
 *  @{
 */

#ifndef __io_h_
#define __io_h_

#if !defined(__ADC_H__)
#include <adc.h>
#endif

#ifndef __general_h__
#include "general.h"
#endif

/* PWM Type Definitions used in Setup_PWM_Output_Type(...) */
#define IFI_PWM   0     /* Standard IFI PWM output generated with Generate_PWM(...) */
#define USER_CCP  1     /* User can use PWM pin as digital I/O or CCP pin. */

/**
 *  Digital I/O direction constants for use with io_set_direction().
 */
typedef enum
{
    /**
     *  Set digital port for output
     */
    IO_DIRECTION_OUT=0,
    
    /**
     *  Set digital port for input
     */
    IO_DIRECTION_IN=1
}   io_dir_t;

/*
 *  I/O pin direction control bits
 */
#define IO_DIRECTION1             TRISAbits.TRISA0
#define IO_DIRECTION2             TRISAbits.TRISA1
#define IO_DIRECTION3             TRISAbits.TRISA2
#define IO_DIRECTION4             TRISAbits.TRISA3
#define IO_DIRECTION5             TRISAbits.TRISA5
#define IO_DIRECTION6             TRISFbits.TRISF0
#define IO_DIRECTION7             TRISFbits.TRISF1
#define IO_DIRECTION8             TRISFbits.TRISF2
#define IO_DIRECTION9             TRISFbits.TRISF3
#define IO_DIRECTION10            TRISFbits.TRISF4
#define IO_DIRECTION11            TRISFbits.TRISF5
#define IO_DIRECTION12            TRISFbits.TRISF6
#define IO_DIRECTION13            TRISHbits.TRISH4
#define IO_DIRECTION14            TRISHbits.TRISH5
#define IO_DIRECTION15            TRISHbits.TRISH6
#define IO_DIRECTION16            TRISHbits.TRISH7

/*
 *  Input pin mappings
 */
#define DIGITAL_IN1     PORTAbits.RA0
#define DIGITAL_IN2     PORTAbits.RA1
#define DIGITAL_IN3     PORTAbits.RA2
#define DIGITAL_IN4     PORTAbits.RA3
#define DIGITAL_IN5     PORTAbits.RA5
#define DIGITAL_IN6     PORTFbits.RF0
#define DIGITAL_IN7     PORTFbits.RF1
#define DIGITAL_IN8     PORTFbits.RF2
#define DIGITAL_IN9     PORTFbits.RF3
#define DIGITAL_IN10    PORTFbits.RF4
#define DIGITAL_IN11    PORTFbits.RF5
#define DIGITAL_IN12    PORTFbits.RF6
#define DIGITAL_IN13    PORTHbits.RH4
#define DIGITAL_IN14    PORTHbits.RH5
#define DIGITAL_IN15    PORTHbits.RH6
#define DIGITAL_IN16    PORTHbits.RH7

/*
 *  Output pin mappings
 */
#define DIGITAL_OUT1    LATAbits.LATA0
#define DIGITAL_OUT2    LATAbits.LATA1
#define DIGITAL_OUT3    LATAbits.LATA2
#define DIGITAL_OUT4    LATAbits.LATA3
#define DIGITAL_OUT5    LATAbits.LATA5
#define DIGITAL_OUT6    LATFbits.LATF0
#define DIGITAL_OUT7    LATFbits.LATF1
#define DIGITAL_OUT8    LATFbits.LATF2
#define DIGITAL_OUT9    LATFbits.LATF3
#define DIGITAL_OUT10   LATFbits.LATF4
#define DIGITAL_OUT11   LATFbits.LATF5
#define DIGITAL_OUT12   LATFbits.LATF6
#define DIGITAL_OUT13   LATHbits.LATH4
#define DIGITAL_OUT14   LATHbits.LATH5
#define DIGITAL_OUT15   LATHbits.LATH6
#define DIGITAL_OUT16   LATHbits.LATH7

/*
 *  Interrupt pin mappings
 */
#define INTERRUPT_IN1   PORTBbits.RB2
#define INTERRUPT_IN2   PORTBbits.RB3
#define INTERRUPT_IN3   PORTBbits.RB4
#define INTERRUPT_IN4   PORTBbits.RB5
#define INTERRUPT_IN5   PORTBbits.RB6
#define INTERRUPT_IN6   PORTBbits.RB7

/*
 *  Below are aliases for the four PWM OUT pins which can be configured
 *  for use as digital inputs or outputs.  They are CCP pins with special
 *  features as detailed in the PIC18FXX20 Data Sheet.
 *
 *   PWM OUT 1 -> CCP2
 *   PWM OUT 2 -> CCP3
 *   PWM OUT 3 -> CCP4
 *   PWM OUT 4 -> CCP5 
 */
 
/*
 *  I/O direction control bits
 */
#define IO_DIRECTION_PWM1       TRISEbits.TRISE7
#define IO_DIRECTION_PWM2       TRISGbits.TRISG0
#define IO_DIRECTION_PWM3       TRISGbits.TRISG3
#define IO_DIRECTION_PWM4       TRISGbits.TRISG4

/*
 *  Input pin mappings
 */
#define DIGITAL_IN_PWM1         PORTEbits.RE7
#define DIGITAL_IN_PWM2         PORTGbits.RG0
#define DIGITAL_IN_PWM3         PORTGbits.RG3
#define DIGITAL_IN_PWM4         PORTGbits.RG4

/*
 *  Output pin mappings
 */
#define DIGITAL_OUT_PWM1        LATEbits.LATE7
#define DIGITAL_OUT_PWM2        LATGbits.LATG0
#define DIGITAL_OUT_PWM3        LATGbits.LATG3
#define DIGITAL_OUT_PWM4        LATGbits.LATG4

/*
 *  Below are aliases for the four PWM OUT pins which can be configured
 *  for use as digital inputs or outputs.  
 *
 *   PWM OUT 5 -> DIGITAL_OUT_PWM05
 *   PWM OUT 6 -> DIGITAL_OUT_PWM06
 *   PWM OUT 7 -> DIGITAL_OUT_PWM07
 *   PWM OUT 8 -> DIGITAL_OUT_PWM08 
 */
 
/*
 *  I/O direction control bits
 */
#define IO_DIRECTION_PWM5        TRISEbits.TRISE0
#define IO_DIRECTION_PWM6        TRISEbits.TRISE1
#define IO_DIRECTION_PWM7        TRISEbits.TRISE2
#define IO_DIRECTION_PWM8        TRISEbits.TRISE3

/*
 *  Output pin mappings
 */
#define DIGITAL_OUT_PWM5       LATEbits.LATE0
#define DIGITAL_OUT_PWM6       LATEbits.LATE1
#define DIGITAL_OUT_PWM7       LATEbits.LATE2
#define DIGITAL_OUT_PWM8       LATEbits.LATE3

#define USART2_TX       LATGbits.LATG1
#define USART2_RX       PORTGbits.RG2

/*
 *  Digital input values
 */
#define DIGITAL_IN_OPEN     1   /* floating high */
#define DIGITAL_IN_CLOSED   0   /* connected to ground */


/* io.c */
status_t io_set_analog_port_count(unsigned char number_of_ports);
unsigned char io_get_analog_port_count(void);
unsigned int io_read_analog(unsigned char port);
char io_read_digital(unsigned char port);
status_t io_write_digital(unsigned char port, unsigned char val);
status_t io_set_direction(unsigned char port, io_dir_t dir);
char    io_get_direction(unsigned char port);
void io_update_local_pwm_dir(unsigned char txPWM_MASK);

#endif

/** @} */


