#include <stdio.h>
#include <usart.h>
#include "platform.h"
#include "vex_usart.h"

/**
 *  \defgroup usart Usart
 *  @{
 */
 
/**
 *  Open the primary usart for output to a terminal.  Serial parameters
 *  will be 115,200 baud, no parity, 8 data bits, and 1 stop bit. (N81)
 *  Lower baud rates may be selected by calling SDCC's usart_open() or
 *  MCC18's OpenUSART() directly.  However, using the maximum baud rate
 *  is generally desirable, since serial output is quite slow, and
 *  may cause the firmware to miss input events.
 */

#ifdef SDCC

/*
 * History:
 *  Dec 2008    Ag Primatic
 */

void    usart_init(void)
{
    usart_open(
	USART_TX_INT_OFF & USART_RX_INT_OFF & USART_BRGH_HIGH
	& USART_ASYNCH_MODE & USART_EIGHT_BIT, BAUD_115200 );
    delay1ktcy(50);
    stdout = STREAM_USART;
}

#else

/*
 * History: 
 *  J. Bacon
 *      Derived from Vex default code.
 *
 * Changes:
 *  J. Bacon
 *      Set stdout=_H_USER to allow use of stdio libraries instead of
 *      printf_lib.c.
 */

void    usart_init(void)
{
    Open1USART(USART_TX_INT_OFF &
	      USART_RX_INT_OFF &
	      USART_ASYNCH_MODE &
	      USART_EIGHT_BIT &
	      USART_CONT_RX &
	      USART_BRGH_HIGH,
	      BAUD_115200);

    Delay1KTCYx(50);    /* Settling time */
    stdout = _H_USER;   /* Direct stdio functions to use _user_putc() */
}


/****************************************************************************
 *  Usart putc used by stdio routines when stdout == _H_USER
 *
 * History:
 *  Dec 2008    J. Bacon
 ***************************************************************************/

void _user_putc(char data)

{
    while ( ! USART_READY )
	;
    TXREG1 = data;
}

/** @} */

#endif

