#include "platform.h"
#include "vex_delay.h"
#include "general.h"

/* FIXME: Make sure all these functions can take delay arguments up to 255 */

/**
 * \defgroup delay Delays
 *  @{
 *
 *  These functions provide a convenient interface for pausing the program
 *  for a given amount of time before proceeding.
 */

/**
 *  Spin delay about ms milliseconds, up to a maximum of 65,536 ms.
 *
 *  \param  ms  Delay in milliseconds
 *
 *  The current implementation will
 *  actually go slightly over since it doesn't account for function
 *  call, loop, and computational overhead.  Each overhead instruction adds
 *  an additional 1/10 microsecond, so typical delays should be accurate
 *  to within a few microseconds.
 *
 *  If you want more accurate delays, use the delay*cty() functions
 *  directly.  Since the VEX delay timer runs at 10MHz, they will produce the
 *  following delays:
 *  
 *  delay10tcy()    1 us \n
 *  delay100tcy()   10 us \n
 *  delay1kcth()    100 us \n
 *  delay10ktcy()   1 ms \n
 *  delay100ktcy()  10 ms \n
 *  delay1mtcy()    100 ms \n
 *
 *  However, be aware that all the delay*tcy() functions
 *  take an unsigned char argument, so they are limited to a maximum
 *  of 255 time units.
 */


void    delay_msec(unsigned int ms)

{
    unsigned char    delay;
    
    /* Should be adjusted to account for function call and loop overhead */
    while ( ms > 0 )
    {
	delay = MIN(ms, 255);
	delay10ktcy(delay);
	ms -= delay;
    }
}


/**
 *  Spin delay about seconds seconds, up to a maximum of 65,536.
 *
 *  \param  seconds Delay in seconds
 *
 *  The current implementation will
 *  actually go slightly over since it doesn't account for function
 *  call, loop, and computational overhead.  Each overhead instruction adds
 *  an additional 1/10 microsecond, so typical delays should be accurate
 *  to within a few microseconds.
 *
 *  If you want more accurate delays, use the delay*cty() functions
 *  directly.  Since the VEX delay timer runs at 10MHz, they will produce the
 *  following delays:
 *  
 *  delay10tcy()    1 us \n
 *  delay100tcy()   10 us \n
 *  delay1kcth()    100 us \n
 *  delay10ktcy()   1 ms \n
 *  delay100ktcy()  10 ms \n
 *  delay1mtcy()    100 ms \n
 *
 *  However, be aware that all the delay*tcy() functions
 *  take an unsigned char argument, so they are limited to a max of 255.
 */

void    delay_sec(unsigned int seconds)

{
    unsigned char    delay;
    
    /* Should be adjusted to account for function call and loop overhead */
    while ( seconds > 0 )
    {
	delay = MIN(seconds, 25);
	delay1mtcy(10*delay);
	seconds -= delay;
    }
}

/** @} */

