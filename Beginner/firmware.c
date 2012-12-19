/**************************************************************************
*
*   This project contains free simple code for the VEX robotics system.
*   It is provided as an aid to students, mentors, and hobbyists
*   for the purpose of programming VEX robots.
*
*   This code is based in part on the VEX default code, and Sutekh's open
*   source VEX library.  The goal of this project is to provide source
*   code for VEX robots which:
*
*   1.  Is easy to read and modify
*   2.  Provides a positive example of software engineering practices for
*       students to learn from.  This includes:
*
*       a.  Consistency in naming conventions and descriptive variable
*           and type names.
*       b.  An API (application program interface) layer between the
*           application and hardware access.  This layer hides the
*           details of hardware access, provides a forward-compatible
*           interface for new programs, and performs sanity checking.
*
*   This file should use the VEX application program interface.
*   All hardware access in this file goes through API functions such as
*   pwm_write(), io_set_digitial(), rc_read_data(), etc.  If you
*   want to code at a lower level, you shoud enhance the library
*   code in the Lib directory and create more API functions to use
*   here if necessary.
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
***************************************************************************/ 

#include <OpenVex.h>
#include "firmware.h"

/****************************************************************************
 * Description:
 *  Main robot loop.
 *
 * History:
 *  Dec 2008    J Bacon
 ***************************************************************************/

void    main(void)

{
    /* 
     *  Gory hardware initialization.  This will set the controller
     *  to a reasonable default state.
     *
     *  Must be done first!
     */
    controller_init();
    
    /* Initialize ports, etc. for this robot configuration. */
    custom_init();

    /*
     *  If digital input port COMPETITION_JUMPER_PORT is jumpered, then
     *  the robot should run autonomous mode for the given number of
     *  seconds as soon as a signal is detected from the RC.
     *
     *  Field control should enable the RC momentarily to trigger
     *  this code to start autonomous, disable it for the remainder
     *  of the autonomous period, and re-enable it for teleoperated mode.
     */
    if ( io_read_digital(COMPETITION_JUMPER_PORT) == 0 )
    {
	/* Wait for RC signal */
	printf("Waiting for RC signal to begin autonomous period...\n");
	while ( rc_read_status() != 1 )
	    ;
	
	autonomous_routine_competition(20);
    }

    /* 
     *  Main loop.  Watch for the master processor to indicate
     *  that new data is available.
     */
    while (TRUE)
    {
	/*
	 *  This program runs on the VEX "user" processor. The VEX
	 *  "master" processor monitors the remote control and
	 *  generates PWMs (pulse-width modulation for motor power).  It
	 *  interrupts the user processor every 18.5ms to signal the
	 *  availability of new data.
	 *
	 *  Code that processes new data from the master processor
	 *  should go inside this if statement.  The if condition will
	 *  only evaluate to true every 18.5ms, so this loop "spins"
	 *  a lot waiting for new data, unless
	 *  process_master_data() takes up a good chunk of 18.5ms.
	 *  If it takes more than 18.5ms, data from the master processor
	 *  will be lost.  ( May or may not be a big deal. )
	 */
	
	if ( rc_new_data_available() )
	{
	    rc_routine();
	    
	    /* Run autonomous routine when a RC lower button is pressed. */
	    if ( rc_read_data(AUTONOMOUS_TRIGGER_CHAN) == -127 )
		autonomous_routine0();
	} 
    }
}


/****************************************************************************
 * Description: 
 *  This function initializes the controller according to the needs of
 *  this program.  Modify it to suite the needs of your robot.
 *
 * History: 
 *  Dec 2008    J Bacon
 ***************************************************************************/

void    custom_init(void)

{
    int     c;

    /*
     *  Below are custom configurations for your robot.  This is where
     *  you set up digital output ports, analog ports, sensors, etc.
     */
     
    /* Set some of the 16 I/O ports as inputs */
    for (c = 1; c <= 16; ++c)
	io_set_direction(c, IO_DIRECTION_IN);

    /*
     *  Configure the number of analog input ports. Ports 1 through N
     *  will be analog, and N+1 through 16 will be digital.
     */
    io_set_analog_port_count(ANALOG_PORTS);

    /* Set some of the 16 I/O ports as outputs (digital output only). */
    /* These can be used to drive LEDs and other low-current devices. */
    //for (c = 9; c <= 16; ++c)
    //    io_set_direction(c, IO_DIRECTION_OUT);

    /* Initialize the values on the digital outputs. */
    //for (c = 9; c <= 16; ++c)
    //    io_set_digital(c,0);
    
    controller_print_version();
}


/****************************************************************************
 * Description: 
 *  Process data from master controller.
 ***************************************************************************/

void    rc_routine(void)

{ 
    /*
     *  Keep a count of calls to this function.  This will overflow
     *  at 2^16 (65, 536), but we don't really care since we're only
     *  interested in whether or not calls is a multiple of some number.
     */ 
    static unsigned int     sonar_distance = 0;
    static unsigned long    elapsed_time,
			    old_time = 0;

    /*
     *  Alternative to default code behavior for robots with TANK
     *  drive (wheels driven directly by motors, no gearing).
     */
    if ( io_read_digital(ARCADE_JUMPER_PORT) == 0 )
	arcade_drive_routine();
    else
	tank_drive_routine();

    controller_submit_data(NO_WAIT);

    /*
     *  Since serial output is slow, we only display once per second
     *  so that the program won't be constantly waiting for terminal
     *  output to finish.
     */

    elapsed_time = SYSTEM_TIMER_SECONDS();
    //printf("TIMER0 = %ld  elapsed_time = %ld  overflows = %d\n",
    //   timer0_read32(), elapsed_time, Timer0_overflows);
    if ( elapsed_time > old_time )
    {
	old_time = elapsed_time;
#if DEBUG_STACK
	stack_report();
#endif

	/* 
	 *  Display status of various sensors and rc inputs.
	 */
	DPRINTF("ET: %ld  RC: %d %d %d %d %d %d %d  Jumper: %d\n",
	    elapsed_time, rc_read_status(),
	    rc_read_data(1), rc_read_data(2),
	    rc_read_data(3), rc_read_data(4),
	    rc_read_data(5), rc_read_data(6),
	    io_read_digital(ARCADE_JUMPER_PORT));
    }
}


/****************************************************************************
 * Description: 
 *  VEX code for a tank-drive RC robot.
 *  
 * History: 
 *  Dec 2008    J Bacon
 ***************************************************************************/

void    tank_drive_routine(void)

{
    static char left_power,
		right_power,
		impeller_power,
		arm_power;
    
    /* Drive motors face opposite directions, so reverse one side. */
    left_power = rc_read_data(TANK_DRIVE_LEFT_CHAN);
    right_power = -rc_read_data(TANK_DRIVE_RIGHT_CHAN);
    
    /* Drive motors */
    pwm_write(RIGHT_DRIVE_PORT, right_power);
    pwm_write(LEFT_DRIVE_PORT, left_power);
    
    /* Implements */
    impeller_power = rc_read_data(TANK_IMPELLER_CHAN);
    arm_power = rc_read_data(TANK_ARM_CHAN);
    pwm_write(IMPELLER_PORT, impeller_power);
    pwm_write(ARM_PORT, arm_power);
}


/****************************************************************************
 * Description: 
 *  VEX code for a arcade-drive RC robot.
 *  
 * History: 
 *  June 2009   J Bacon
 ***************************************************************************/

void    arcade_drive_routine(void)

{
    char        left_power,
		right_power,
		impeller_power,
		arm_power,
		joy_x,
		joy_y;
    
    joy_x = rc_read_data(ARCADE_DRIVE_X_CHAN);
    joy_y = -rc_read_data(ARCADE_DRIVE_Y_CHAN);
    arcade_drive(joy_x, joy_y, PWM_MAX, &left_power, &right_power);
    // printf("%d %d %d %d\n", joy_x, joy_y, left_power, right_power);

    /* Drive motors */
    pwm_write(LEFT_DRIVE_PORT, -left_power);
    pwm_write(RIGHT_DRIVE_PORT, right_power);
    
    /* Implements */
    impeller_power = rc_read_data(ARCADE_IMPELLER_CHAN);
    arm_power = rc_read_data(ARCADE_ARM_CHAN);
    pwm_write(IMPELLER_PORT, impeller_power);
    pwm_write(ARM_PORT, arm_power);
}


/***************************************************************************
 *  Description:
 *      Run the robot autonomously.
 *
 *  Arguments:
 *      None.
 *
 *  History: 
 *      Jan 2010    J Bacon     Created stub.
 ***************************************************************************/


void    autonomous_routine0(void)

{
    DPRINTF("Starting autonomous routine...\n");
    controller_begin_autonomous_mode();
    
    /*
     *  Place your autonomous code here.  The example below drives
     *  the robot forward (or backward) for 2 seconds and then stops.
     */
    
    pwm_write(RIGHT_DRIVE_PORT, AUTON_DRIVE_SPEED);
    pwm_write(LEFT_DRIVE_PORT, -AUTON_DRIVE_SPEED);
    controller_submit_data(WAIT);
    delay_sec(2);
    pwm_write(RIGHT_DRIVE_PORT, MOTOR_STOP);
    pwm_write(LEFT_DRIVE_PORT, MOTOR_STOP);
    controller_submit_data(WAIT);
    
    controller_end_autonomous_mode();
    DPRINTF("Ending autonomous routine...\n");
}


void    autonomous_routine_competition(unsigned short seconds)

{
    printf("Running competition autonomous routine...\n");
    controller_begin_autonomous_mode();

    /*
     *  Run for the length of the autonomous period (usually 20 seconds).
     */

    /* Example
    pwm_write(LEFT_FRONT_DRIVE_PORT, 80);
    pwm_write(LEFT_REAR_DRIVE_PORT, 80);
    controller_submit_data(WAIT);
    delay_sec(2);
    pwm_write(LEFT_FRONT_DRIVE_PORT, 0);
    pwm_write(LEFT_REAR_DRIVE_PORT, 0);
    controller_submit_data(WAIT);
    */
    
    controller_end_autonomous_mode();
}

