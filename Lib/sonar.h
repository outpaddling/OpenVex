
#ifndef __sonar_h_
#define __sonar_h_

#ifndef __general_h__
#include "general.h"
#endif

#define SONAR_PULSE_DURATION    0xC0

/*
 *  Distance is proportional to echo time.  The measured echo time includes
 *  a small amount of overhead, which must be subtracted off before
 *  dividing.  The subtrahend and divisor below are for raw clock
 *  cycles, and should work for any prescale value.
 *  ( Note that (t) is multiplied by the prescale factor to eliminate
 *  timer prescaling from the computations. )
 *  Overhead time was impericaly determined using a meter stick to calibrate,
 *  and sound travels 1 cm in 29.15us (291.5 clock cycles).  Hence, round
 *  trip time per cm is 583 clock cycles.
 */
#define ECHO_TIME_TO_CM(t)      ( ((unsigned long)TIMER0_PRESCALE * (t) - 400) / 583 )

extern volatile unsigned char   Sonar_data_available[];
extern unsigned char            Sonar_on_iport[];

/* 
 *  These are used in ISRs, so we use macros for speed.  MCC18 doesn't
 *  support inline functions.
 */
#define SONAR_ON_IPORT(port)        (Sonar_on_iport[(port)-1])
#define SET_SONAR_ON_IPORT(port)    (Sonar_on_iport[(port)-1] = 1)
#define CLR_SONAR_ON_IPORT(port)    (Sonar_on_iport[(port)-1] = 0) 

#define SONAR_DATA_AVAILABLE(port)      (Sonar_data_available[(port)-1])
#define SET_SONAR_DATA_AVAILABLE(port)  (Sonar_data_available[(port)-1] = 1)
#define CLR_SONAR_DATA_AVAILABLE(port)  (Sonar_data_available[(port)-1] = 0)

#define SONAR_ECHO_TIME(port)           (Sonar_echo_time[(port)-1])
#define SET_SONAR_ECHO_TIME(port,v)     (Sonar_echo_time[(port)-1] = (v))
#define CLR_SONAR_ECHO_TIME(port,v)     (Sonar_echo_time[(port)-1] = (v))

/* sonar.c */
status_t sonar_init_spin_wait(unsigned char port_to_sonar, unsigned char port_from_sonar);
unsigned int sonar_read_spin_wait(unsigned char port_to_sonar, unsigned char port_from_sonar);
status_t sonar_init(unsigned char digital_port_to_sonar, unsigned char interrupt_port_from_sonar);
unsigned int sonar_read(unsigned char interrupt_port_from_sonar);
void sonar_emit_isr(unsigned char interrupt_port_from_sonar);
void sonar_echo_isr(unsigned char interrupt_port_from_sonar);
status_t sonar_emit_pulse(unsigned char port_to_sonar);
status_t sonar_emit_pulse_interrupt(unsigned char digital_port_to_sonar, unsigned char interrupt_port_from_sonar);
void sonar_init_timer0(void);
int sonar_data_available(unsigned char interrupt_port_from_sonar);

#endif

