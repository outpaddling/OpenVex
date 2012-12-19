#include <OpenVex.h>

/****************************************************************************
 * Description:
 *  Simplest possible OpenVex program.
 *
 * History:
 *  Aug 2009    J Bacon
 ***************************************************************************/

#define TRUE    1

void    main(void)

{
    /* 
     *  Gory hardware initialization.  This will set the controller
     *  to a reasonable default state.
     *
     *  Must be done first!
     */
    controller_init();
    
    /*
     *  To keep the Vex master code happy, you must either
     *  a.  Enter autonomous mode using
     *      controller_begin_autonomous_mode();
     *  b.  Run controller_submit_data() periodically
     */

    /* RC mode main loop */
    while (TRUE)
    {
	controller_submit_data(NO_WAIT);
	printf("Hi, Bob!\n");
	delay_sec(1);
    }
}

