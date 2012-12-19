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
*       Created Dec 2008    Jason W. Bacon
***************************************************************************/

#ifndef __shaft_encoder_h__
#define __shaft_encoder_h__

#ifndef __general_h__
#include "general.h"
#endif

extern unsigned char            Encoder_on_iport[6];
extern volatile unsigned int    Encoder_ticks[6];
extern unsigned char            Quad_input_port[6];
extern volatile unsigned char   Porta_sample, Portf_sample, Porth_sample;

#define ENCODER_STD     1
#define ENCODER_QUAD    2
#define ENCODER_MAX_SHAFTS  TOTAL_INTERRUPT_PORTS

/* These are used in ISRs, so they're implemented as macros for speed */
#define ENCODER_ON_IPORT(port)          (Encoder_on_iport[(port)-1])
#define SET_ENCODER_ON_IPORT(port,type) (Encoder_on_iport[(port)-1] = (type))
#define CLR_ENCODER_ON_IPORT(port)      (Encoder_on_iport[(port)-1] = 0)

#define SHAFT_ENCODER_ISR(p)            (++Encoder_ticks[(p)-1])

#define SHAFT_ENCODER_READ_STD(p)       (Encoder_ticks[(p)-1])
#define SHAFT_ENCODER_READ_QUAD(p)      ((int)Encoder_ticks[(p)-1])

typedef struct
{
    // Time limit in ms, 0 means indefinite
    unsigned long   timer_limit;
    
    // Rotation limit in encoder ticks, 0 means indefinite
    unsigned short  tick_limit;
    
    /*
     *  Max for shaft encoders is 1133 rpm
     *
     *  Ticks   rotations   90 ticks     1 minute
     *  ----- = --------- x ---------- x --------
     *  1 sec   1 minute    1 rotation   60 sec
     *
     *  Note: SDCC 2.9 PIC port limits bit fields to 8 bits, so we
     *  use a whole short for rpm instead of the minimum 12 bits.
     */
    short           tps;            /* PID Process variable */
    
    unsigned short  motor_port:3;
    unsigned short  leave_encoder_on:1;
    unsigned short  interrupt_port:4;
    unsigned short  input_port:4;

    short           integral;
    char            previous_error;
    char            power;          /* PID Manipulated variable */
}   shaft_t;

/* shaft_encoder.c */
status_t shaft_encoder_enable(unsigned char port);
status_t shaft_encoder_disable(unsigned char port);
status_t shaft_encoder_reset(unsigned char port);
unsigned int shaft_encoder_read_std(unsigned char port);
void shaft_encoder_isr(unsigned char port);
void quad_encoder_isr(unsigned char port);
int     shaft_encoder_read_quad(unsigned char port);
status_t    shaft_encoder_enable_std(unsigned char interrupt_port);
status_t    shaft_encoder_enable_quad(unsigned char interrupt_port,unsigned char input_port);
status_t    shaft_tps_run(shaft_t shafts[], unsigned char count);
status_t    shaft_tps_init(shaft_t *sp,
		    unsigned long timer_limit, unsigned short tick_limit,
		    unsigned char motor_port, unsigned char shaft_interrupt_port,
		    unsigned char shaft_input_port, short tps);
#endif

