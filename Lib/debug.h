
#ifndef __debug_h__
#define __debug_h__

#define DEBUG_STACK     0
#define DEBUG_SPI       0

/**
 *  Each DPRINTF() is converted to printf() if the constant
 *  DEBUG is 1.  If DEBUG is 0, the entire DPRINTF() statement is removed
 *  by the preprocessor, cpp.
 *  Hence, by switching DEBUG from 1 to 0 and recompiling, you effectively
 *  remove all DPRINTF() statements from your program, making the executable
 *  smaller and faster.
 */

#if DEBUG
#define DPRINTF(...)    printf(__VA_ARGS__)
#else
#define DPRINTF(...)
#endif

#if DEBUG
#define DEBUG_VALIDATE_RANGE(...)    debug_validate_range(__VA_ARGS__)
#else
#define DEBUG_VALIDATE_RANGE(...)
#endif

/* debug.c */
void debug_paint_stack_bank(void);
void debug_stack_report(void);
void debug_spi_buff_dump(unsigned char *);
void debug_hex_dump(unsigned char *, unsigned int);
void    debug_validate_range(const char *function_name, const char *arg_name, 
			    int value, int min, int max);
#endif

