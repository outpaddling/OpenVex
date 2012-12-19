#include <stdio.h>
#include "platform.h"
#include "debug.h"

/**
 * \defgroup debug Debug
 *  @{
 *
 *  These macros and functions are provided to assist with debugging OpenVex programs.
 */

/**
 *  Initialize all unused stack space to a known value.  Afterward, we can
 *  use debug_stack_report() to see how many markers were overwritten.  This
 *  function should be called early from main (but after controller_init())
 *  in order to paint as much of the stack as possible.  If called from
 *  a function deeper in the call tree, more stack will be in use, and
 *  less will be painted with the sentinal value.
 */

/*
 * History:
 *  Dec 2008    Ag Primatic, J Bacon
 */

void    debug_stack_paint(void)

{
    long DATA   *stack = STACK_BASE;
    int         ii;

    /*
     *  Report initial stack pointer location.  This may create a
     *  high-water mark.
     */
    // printf("FSR1 = %x %x\n",FSR1H,FSR1L);
    
    /*
     *  If we used a 1-byte value, there would be a 1/256 chance
     *  that the value the program placed at the top of the stack is our
     *  marker, leading to an erroneous report.  Hence, we use a 4-byte
     *  marker, which has only a 1 in 4.2 billion chance.  A longword 
     *  measurement is accurate enough for this purpose, and very reliable.
     */
#ifdef SDCC
    for (ii = 0; ii * sizeof(long) < FSR1L-4; ++ii)
	stack[ii] = 0xdeadbeef;
#else
    for (ii = 63; ii * sizeof(long) > FSR1L+4; --ii)
	stack[ii] = 0xdeadbeef;
#endif
}


/**
 *  Report maximum stack usage so far (high water mark).  The stack must
 *  have been initialized with debug_stack_paint().  Note that printf()
 *  is likely to create a high-water mark (it calls a chain of lower level
 *  functions down to the device-specific putc()), so if you want to know the
 *  true stack requirements of your code, avoid doing any other debug output
 *  before calling this function.  Also, as this function itself may
 *  create a high-water mark when it calls printf(), it may be that only
 *  the first invocation provides meaningful data.
 *
 * Note:
 *  SDCC also has a number of stack debugging options such as --stack-probe
 *  and --gstack which can provide more detailed information.
 */

/*
 * History:
 *  Dec 2008    Ag Primatic, J Bacon
 */

void    debug_stack_report(void)

{
    long DATA *stack = STACK_BASE;
    int     ii,
	    size;

#ifdef SDCC
    if (stack[0] != 0xdeadbeef) {
	printf("Stack overflowed\r\n");
	while (1)
	    ;
    }

    for (ii = 1; stack[ii] == 0xdeadbeef; ii++)
	;
    size = 64 - ii;
#else
    for (ii = 63; stack[ii] == 0xdeadbeef; ii--)
	;
    size = ii;
#endif
    printf("FSR1 = %x %x Max stack usage so far = %d/64 longs.\n",
	FSR1H, FSR1L, size, stack[ii]);
}


/**
 *  Print count bytes of data starting from in hexadecimal.
 */

void    debug_hex_dump(unsigned char *address, unsigned int count)

{
    while ( count-- )
	printf("%x ", *address++);
    printf("\n");
}


/**
 *  Print the contents of an SPI buffer as a 4x8 block of hex values.
 *  This makes it easier to pick out particular values than a 1x32
 *  hex dump.
 */

void    debug_spi_buff_dump(unsigned char *ptr)

{
    int ii;

    for (ii = 0; ii < 4; ii++) {
	   DPRINTF("%d: %x %x %x %x %x %x %x %x\n", 8*ii,
		  ptr[8*ii+0],
		  ptr[8*ii+1],
		  ptr[8*ii+2],
		  ptr[8*ii+3],
		  ptr[8*ii+4],
		  ptr[8*ii+5],
		  ptr[8*ii+6],
		  ptr[8*ii+7]
		  );
    }
    DPRINTF("\n");
}

/** @} */

