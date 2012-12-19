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

#ifndef __interrupts_h__
#define __interrupts_h__

#define INTERRUPT_FALLING_EDGE  0
#define INTERRUPT_RISING_EDGE   1

/*
 *  Interrupt vectors for ISR definitions.  The PIC18F* processors have
 *  only reset interrupts plus high and low priority interrupts for
 *  general use.  Example:
 *
 *  void    isr(void) interrupt INTERRUPT_LOW
 *  {
 *  }
 */
#define INTERRUPT_RESET         0
#define INTERRUPT_HIGH          1
#define INTERRUPT_LOW           2

/* These are used in ISRs, so they're implemented as macros for speed */
#define INTERRUPT_PORT_IN_USE(port)     (Interrupt_port_in_use[(port)-1])
#define SET_INTERRUPT_PORT_IN_USE(port) (Interrupt_port_in_use[(port)-1] = 1)
#define CLR_INTERRUPT_PORT_IN_USE(port) (Interrupt_port_in_use[(port)-1] = 0)

extern unsigned char    Interrupt_port_in_use[];

/* interrupts.c */
void interrupt_set_edge(unsigned char port, unsigned char mask);
void interrupt_enable(unsigned char port);
void interrupt_disable(unsigned char port);
#ifdef SDCC
void InterruptVectorLow(void) NAKED_INTERRUPT_VECTOR(2);
void InterruptHandlerLow(void) INTERRUPT;
#else
void InterruptVectorLow(void);
void InterruptHandlerLow(void);
#endif

#endif

