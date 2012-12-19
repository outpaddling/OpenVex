/**************************************************************************
*
*   This project contains free sample code for the VEX robotics system.
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
#if ENABLE_AUTONOMOUS_ROUTINES
	    /*
	     *  Autonomous routines are triggered by a binary
	     *  sequence on button AUTO_TRIGGER_BUTTON.
	     *  Down = 0, Up = 1.  00 runs
	     *  autonomout_routine0(), 01 runs autonomous_routine1(),
	     *  etc.
	     */
	    switch( rc_button_sequence(AUTO_TRIGGER_BUTTON, AUTO_TRIGGER_SEQ_LEN) )
	    {
		case    0:
		    autonomous_routine0();
		    break;
		case    1:
		    //autonomous_routine1();
		    break;
		case    2:
		    autonomous_routine2();
		    break;
		case    3:
		    //autonomous_routine3();
		    break;
		default:
		    break;
	    }
#endif  /* ENABLE_AUTONOMOUS_ROUTINES */
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

    /* Enable shaft encoder on any interrupt port */
    /* The count can be read back with shaft_encoder_read_count() */
    shaft_encoder_enable_std(LEFT_ENCODER_INTERRUPT_PORT);
    shaft_encoder_enable_std(RIGHT_ENCODER_INTERRUPT_PORT);
    
    /* Example: Quad encoder requires two pots */
    // quad_encoder_enable(6, 8);
    
    /* Enable sonar device and timer. */
    if ( sonar_init(SONAR_INTERRUPT_PORT, SONAR_OUTPUT_PORT) != OV_OK )
	printf("sonar_init() failed.\n");

    timer_start(1);
    
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
    static line_sensor_t    line_sensor;
    
    sonar_distance = sonar_read(SONAR_INTERRUPT_PORT);

#if DRIVE_ROUTINE == DEFAULT
    /*
     *  VEX default code behavior.  The default code assumes a squarebot
     *  or similar configuration.
     *  Replace this with your own function to implement different behavior.
     */
    vex_default_routine(User_rxdata,txdata);
#elif DRIVE_ROUTINE == TANK
    /*
     *  Alternative to default code behavior for robots with tank
     *  drive (wheels driven directly by motors, no gearing).
     */
    tank_drive_routine();
#elif DRIVE_ROUTINE == ARCADE
    arcade_drive_routine();

#endif

    controller_submit_data(NO_WAIT);

    /*
     *  Since serial output is slow, we only display once per second
     *  so that the program won't be constantly waiting for terminal
     *  output to finish.
     */

    elapsed_time = SYSTEM_TIMER_SECONDS();

    if ( elapsed_time > old_time )
    {
	old_time = elapsed_time;
#if DEBUG_STACK
	stack_report();
#endif

	line_sensor_read(LINE_SENSOR_LEFT_PORT, LINE_SENSOR_CENTER_PORT,
			LINE_SENSOR_RIGHT_PORT, &line_sensor);
			
	/* 
	 *  Display status of various sensors and rc inputs.
	 */
	DPRINTF("ET: %ld  RC: %d %d %d %d %d %d %d  Line: %x %x %x Light %x ",
	    elapsed_time, rc_read_status(),
	    rc_read_data(1), rc_read_data(2),
	    rc_read_data(3), rc_read_data(4),
	    rc_read_data(5), rc_read_data(6),
	    line_sensor.left, line_sensor.center, line_sensor.right,
	    io_read_analog(LIGHT_SENSOR_PORT));
	
	DPRINTF("B: %d %d  Shaft I[%d,%d]: %d %d  Sonar[%d]: %d T1: %ld\n",
	    io_read_digital(BUMPER_LEFT_PORT),
	    io_read_digital(BUMPER_RIGHT_PORT),
	    LEFT_ENCODER_INTERRUPT_PORT, RIGHT_ENCODER_INTERRUPT_PORT,
	    shaft_encoder_read_std(LEFT_ENCODER_INTERRUPT_PORT),
	    shaft_encoder_read_std(RIGHT_ENCODER_INTERRUPT_PORT),
	    SONAR_INTERRUPT_PORT, sonar_distance, timer_read_ms(1));
    }
}


#if DRIVE_ROUTINE == DEFAULT
/****************************************************************************
 * Description: 
 *  VEX default code for configurations A, B and C.
 *  (B = drive mode 12 - settings on radio)
 *  
 * History: 
 *  Dec 2008    J Bacon     Derived from Vex default code.
 ***************************************************************************/

void    vex_default_routine(void)

{
    pwm_write(RIGHT_DRIVE_PORT, rc_read_data(1));
    pwm_write(LEFT_DRIVE_PORT, rc_read_data(2));
    /* Drive motors face opposite directions, so reverse left side. */
    pwm_write(3, -rc_read_data(3));
    pwm_write(4, rc_read_data(4));

    /* Handle Channel 5 receiver button */
    if (rc_read_data(5) < BUTTON_REV_THRESH)
	pwm_write(5, MOTOR_FULL_FWD);
    else if (rc_read_data(5) > BUTTON_FWD_THRESH)
	pwm_write(5, MOTOR_STOP);
    else
	pwm_write(5, MOTOR_STOP);
	
    /* Handle Channel 6 receiver button */
    if (rc_read_data(6) < BUTTON_REV_THRESH)
	pwm_write(6, MOTOR_FULL_FWD);
    else if (rc_read_data(6) > BUTTON_FWD_THRESH)
	pwm_write(6, MOTOR_STOP);
    else
	pwm_write(6, MOTOR_STOP);

    /* When Jumper 15 is on CONFIGURATION C is selected */
    if (io_read_digital(15) == DIGITAL_IN_CLOSED)
    {
	/* CONFIGURATION C */
	pwm_write(7, pwm_get(2));
	pwm_write(8, pwm_get(3));
    }
    else
    {
	/* CONFIGURATION A & B */
	pwm_write(7, ~pwm_get(5));
	pwm_write(8, ~pwm_get(6));
    } 
}
#endif  /* DRIVE_ROUTINE == DEFAULT */

#if DRIVE_ROUTINE == TANK
/****************************************************************************
 * Description: 
 *  VEX code for a tank-drive RC robot.
 *  
 * History: 
 *  Dec 2008    J Bacon
 ***************************************************************************/

void    tank_drive_routine(void)

{
    static char rc_pos,
		implement_pos = SERVO_CENTER,
		left_drive,
		right_drive;
    
    left_drive = rc_read_data(LEFT_TANK_DRIVE_CHAN);
    right_drive = -rc_read_data(RIGHT_TANK_DRIVE_CHAN);
    
    /*
     *  This code is for a forklift implement mounted on a servo.
     *  If forklift is below level, digitally "gear down" the drive motors
     *  for finer control of the robot while trying to slide forks
     *  under a pallet.
     */
    if ( implement_pos > SERVO_CENTER )
    {
	left_drive /= IMPLEMENT_GEAR_DOWN;
	right_drive /= IMPLEMENT_GEAR_DOWN;
    } 
    
    /* Drive motors */
    pwm_write(LEFT_DRIVE_PORT, left_drive);
    pwm_write(RIGHT_DRIVE_PORT, right_drive);
    
    /*
     *  Servo controlling implement on port 4
     *  If joystick is off center by the
     *  threshold value (to avoid moving due to joystik trim issues and
     *  inadvertent lateral movements) and the servo
     *  position is within bounds, move servo one position up or down.
     */
    rc_pos = rc_read_data(IMPLEMENT_CHAN);
    if ( (rc_pos < JOY_CENTER - JOY_THRESHOLD) &&
	 (implement_pos < IMPLEMENT_DOWN) )
	++implement_pos;
    else if ( (rc_pos > JOY_CENTER + JOY_THRESHOLD) &&
	      (implement_pos > IMPLEMENT_UP) )
	--implement_pos;
    pwm_write(IMPLEMENT_SERVO_PORT, implement_pos);
}
#endif  /* DRIVE_ROUTINE == TANK */


#if DRIVE_ROUTINE == ARCADE
/****************************************************************************
 * Description: 
 *  VEX code for a arcade-drive RC robot.
 *  
 * History: 
 *  June 2009   J Bacon
 ***************************************************************************/

void    arcade_drive_routine(void)

{
    static char rc_pos,
		implement_pos = SERVO_CENTER;
    static char left_drive,
		right_drive,
		joy_x,
		joy_y;
    
    joy_x = rc_read_data(ARCADE_DRIVE_X_CHAN);
    joy_y = -rc_read_data(ARCADE_DRIVE_Y_CHAN);
    arcade_drive(joy_x, joy_y, PWM_MAX, &left_drive, &right_drive);
    // printf("%d %d %d %d\n", joy_x, joy_y, left_drive, right_drive);

    /*
     *  This code is for a forklift implement mounted on a servo.
     *  If forklift is below level, digitally "gear down" the drive motors
     *  for finer control of the robot while trying to slide forks
     *  under a pallet.
     */
    if ( implement_pos > SERVO_CENTER )
    {
	left_drive /= IMPLEMENT_GEAR_DOWN;
	right_drive /= IMPLEMENT_GEAR_DOWN;
    } 
    
    /* Drive motors */
    pwm_write(LEFT_DRIVE_PORT, -left_drive);
    pwm_write(RIGHT_DRIVE_PORT, right_drive);
    
    /*
     *  Servo controlling implement on port 4
     *  If joystick is off center by the
     *  threshold value (to avoid moving due to joystik trim issues and
     *  inadvertent lateral movements) and the servo
     *  position is within bounds, move servo one position up or down.
     */
    rc_pos = -rc_read_data(ARCADE_IMPLEMENT_CHAN);
    if ( (rc_pos < JOY_CENTER - JOY_THRESHOLD) &&
	 (implement_pos < IMPLEMENT_DOWN) )
	++implement_pos;
    else if ( (rc_pos > JOY_CENTER + JOY_THRESHOLD) &&
	      (implement_pos > IMPLEMENT_UP) )
	--implement_pos;
    pwm_write(IMPLEMENT_SERVO_PORT, implement_pos);
}
#endif  /* DRIVE_ROUTINE == ARCADE */


/*******************************************************************************************
 * Description:
 *  Move sonar mounted on servo side to side.  Each call to this function
 *  turns the servo 2 degrees in the current direction.  The function
 *  keeps track of direction and reverses when the angle limit is reached.
 *
 * History: 
 *  May 2009    J Bacon 
 *******************************************************************************************/

void    sonar_scan(unsigned char port)

{
    static char sonar_direction = SERVO_CENTER;
    static char add = 2;
    
    /* Servo oscillating sonar direction */
    pwm_write(port, sonar_direction);
    sonar_direction += add;
    
    /* Reverse when position reaches limit */
    if ( (sonar_direction == -SONAR_MAX_ANGLE) ||
	 (sonar_direction == +SONAR_MAX_ANGLE) )
	add = -add;
    controller_submit_data(NO_WAIT);
}


#if ENABLE_AUTONOMOUS_ROUTINES
/*******************************************************************************************
 * Description:
 *  Simple routine to drive robot in a square pattern.
 *
 * History: 
 *  May 2009    J Bacon
 *  Nov 2009    J Bacon     Rewrote using shaft_tps routines.
 *******************************************************************************************/

void    autonomous_routine0()

{
    int             c;
    shaft_t         shafts[2];
    
    controller_begin_autonomous_mode();

    for (c=0; c<4; ++c)
    {
	/* Drive forward for a while */
	if ( shaft_tps_init(&shafts[0], 0, 180,
	    LEFT_DRIVE_PORT, LEFT_ENCODER_INTERRUPT_PORT, 0, -60) != OV_OK )
	    printf("shaft_tps_init() failed.\n");
	shaft_tps_init(&shafts[1], 0, 180,
	    RIGHT_DRIVE_PORT, RIGHT_ENCODER_INTERRUPT_PORT, 0, 60);
	shaft_tps_run(shafts, 2);
	
	pwm_write(LEFT_DRIVE_PORT, MOTOR_STOP);
	pwm_write(RIGHT_DRIVE_PORT, MOTOR_STOP);
	controller_submit_data(WAIT);
    
	/* Turn */
	shaft_tps_init(&shafts[0], 0, 105,
	    LEFT_DRIVE_PORT, LEFT_ENCODER_INTERRUPT_PORT, 0, -60);
	shaft_tps_run(shafts, 1);
    }
    
    controller_end_autonomous_mode();
}


#if 0
/*******************************************************************************************
 * Description:
 *  Drive straight forward/backward, assuming drive motors are equal.
 *******************************************************************************************/

void    forward1(char power)
{
    pwm_write(RIGHT_DRIVE_PORT, +power);
    pwm_write(LEFT_DRIVE_PORT, -power);
    controller_submit_data(WAIT);
}


void    reverse1(char power)
{
    pwm_write(RIGHT_DRIVE_PORT, -power);
    pwm_write(LEFT_DRIVE_PORT, +power);
    controller_submit_data(WAIT);
}


/*******************************************************************************************
 * Description:
 *  Turn robot for given number of seconds.
 *******************************************************************************************/

void    spin1(char power, unsigned short ms)

{
    pwm_write(RIGHT_DRIVE_PORT, +power);
    pwm_write(LEFT_DRIVE_PORT, +power);
    controller_submit_data(WAIT);
    delay_msec(ms);
}

      
/*******************************************************************************************
 * Description:
 *  Simple looping autonomous routine to demonstrate use of sensor input. 
 *
 * History: 
 *  May 2009    J Bacon
 *******************************************************************************************/

void    autonomous_routine1(void)

{
    unsigned int            sonar_distance = 0;
    static unsigned long    elapsed_time,
			    old_time = 0,
			    start_time;
    
    controller_begin_autonomous_mode();
    elapsed_time = start_time = SYSTEM_TIMER_SECONDS();
    
    /*
     *  An autonomous loop with sensor input.  Loop terminates when
     *  a button sensor on port 5 is pressed, or the 20 second time
     *  period runs out.
     */
    
    forward1(MOTOR_POWER1); 
    while ( (elapsed_time - start_time) < ROUTINE1_DURATION )
    {
	sonar_distance = sonar_read(SONAR_INTERRUPT_PORT);
	
	if ( (sonar_distance < ROUTINE1_SONAR_DISTANCE) ||
	     (io_read_digital(BUMPER_LEFT_PORT) == 0) ||
	     (io_read_digital(BUMPER_RIGHT_PORT) == 0) )
	{
	    reverse1(MOTOR_POWER1);
	    delay_msec(ROUTINE1_BACKUP_MS);
	    sonar_distance = sonar_read(SONAR_INTERRUPT_PORT);
	    spin1(MOTOR_POWER1, ROUTINE1_SPIN_MS);
	    forward1(MOTOR_POWER1);
	    sonar_distance = sonar_read(SONAR_INTERRUPT_PORT);
	}
	
	elapsed_time = SYSTEM_TIMER_MS();
	
	/* Adjust sonar direction every 40 ms */
	if ( elapsed_time % ROUTINE1_SONAR_SERVO_DELAY == 0 )
	    sonar_scan(SONAR_SERVO_PORT);
	
	/* Produce debug output once per second */
	elapsed_time /= MS_PER_SEC;
	if ( elapsed_time > old_time )
	{
	    old_time = elapsed_time;
	    /*
	    if ( rc_new_data_available() )
	    {
		DPRINTF("ET: %ld  RC: %x  A[1,2]: %x %x  "
		    "D[5,6]: %d %d  Shaft I[%d,%d]: %d %d Sonar[%d]: %d\n",
		    elapsed_time - start_time, rc_read_status(),
		    io_read_analog(1),  io_read_analog(2),
		    io_read_digital(5), io_read_digital(6),
		    LEFT_ENCODER_INTERRUPT_PORT, RIGHT_ENCODER_INTERRUPT_PORT,
		    shaft_encoder_read_std(LEFT_ENCODER_INTERRUPT_PORT),
		    shaft_encoder_read_std(RIGHT_ENCODER_INTERRUPT_PORT),
		    SONAR_INTERRUPT_PORT, sonar_distance);
	    }
	    */
	}
    }
    pwm_write(RIGHT_DRIVE_PORT, MOTOR_STOP);
    pwm_write(LEFT_DRIVE_PORT, MOTOR_STOP);
    controller_submit_data(NO_WAIT);

    controller_end_autonomous_mode();
}
#endif

/*******************************************************************************************
 * Description:
 *  Simple routine to drive robot in a square pattern.
 *
 * History: 
 *  May 2009    J Bacon
 *  Nov 2009    J Bacon     Rewrote using shaft_tps routines.
 *******************************************************************************************/

void    autonomous_routine2()

{
    shaft_t         shafts[2];
    
    controller_begin_autonomous_mode();

    /* Drive forward for a while */
    shaft_tps_init(&shafts[0], 0, ROUTINE2_TICKS,
	LEFT_DRIVE_PORT, LEFT_ENCODER_INTERRUPT_PORT, 0, -ROUTINE2_POWER);
    shaft_tps_init(&shafts[1], 0, ROUTINE2_TICKS,
	RIGHT_DRIVE_PORT, RIGHT_ENCODER_INTERRUPT_PORT, 0, ROUTINE2_POWER);
    shaft_tps_run(shafts, 2);
    
    pwm_write(LEFT_DRIVE_PORT, MOTOR_STOP);
    pwm_write(RIGHT_DRIVE_PORT, MOTOR_STOP);
    controller_submit_data(WAIT);
    
    /* Drive backward for a while */
    shaft_tps_init(&shafts[0], 0, ROUTINE2_TICKS,
	LEFT_DRIVE_PORT, LEFT_ENCODER_INTERRUPT_PORT, 0, ROUTINE2_POWER);
    shaft_tps_init(&shafts[1], 0, ROUTINE2_TICKS,
	RIGHT_DRIVE_PORT, RIGHT_ENCODER_INTERRUPT_PORT, 0, -ROUTINE2_POWER);
    shaft_tps_run(shafts, 2);
    
    pwm_write(LEFT_DRIVE_PORT, MOTOR_STOP);
    pwm_write(RIGHT_DRIVE_PORT, MOTOR_STOP);
    controller_submit_data(WAIT);
    
    controller_end_autonomous_mode();
}


#if 0
void    autonomous_routine3()

{
    int     acceleration,
	    c;
    long    velocity = 0,
	    position = 0;

    controller_begin_autonomous_mode();
    
    pwm_write(LEFT_DRIVE_PORT, -50);
    pwm_write(RIGHT_DRIVE_PORT, 50);
    controller_submit_data(WAIT);
    
    for (c=0; c<100; ++c)
    {
	read_accelerometer_axis(4, 535, 5000, &acceleration, &velocity, &position);
	delay_msec(10);
    }
    
    pwm_write(LEFT_DRIVE_PORT, MOTOR_STOP);
    pwm_write(RIGHT_DRIVE_PORT, MOTOR_STOP);
    controller_submit_data(WAIT);
    
    controller_end_autonomous_mode();
}
#endif

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
    
    delay_sec(1);
    
    pwm_write(LEFT_FRONT_DRIVE_PORT, 0);
    pwm_write(LEFT_REAR_DRIVE_PORT, 0);
    controller_submit_data(WAIT);
    */
    
    controller_end_autonomous_mode();
}

#endif  /* ENABLE_AUTONOMOUS_ROUTINES */

