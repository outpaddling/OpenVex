/**
 *  \mainpage
 *
 *  OpenVex is an API (Application Program Interface) library for
 *  programming Vex robot controllers using SDCC or MCC18.
 *
 *  The API consists of functions for
 *  devices such as analog sensors, digital sensors, LED output devices,
 *  motors, servos, timers, etc.
 *
 *  The goal of the library is to make programming Vex robots easier
 *  and more organized without sacrificing the near-optimal speed and
 *  flexibility of C.
 *
 *  OpenVex can be used to program the Vex under any operating
 *  system and hardware that runs SDCC.  This includes all major PC operating
 *  systems such as Mac (Intel or PowerPC), most Unix systems, and Windows
 *  (Cygwin recommended).  You can use the editor or IDE (integrated development
 *  environment) of your choice and build the code with the included Makefiles.
 *
 *  The basic structure of an OpenVex program is as follows:
 *  \code
 *  #include <OpenVex.h>
 *
 *  #define LIGHT_THRESHOLD 2000
 *
 *  void    main(void)
 *  { 
 *      // Initialize the Vex controller
 *      controller_init();
 *
 *      // Set up your robot-specific configuration
 *      custom_init();
 *
 *      // Main loop: Begins in RC (remote control) mode.
 *      while ( TRUE )
 *      {
 *          // Possibly check sensors here
 *          if ( io_read_analog(LIGHT_SENSOR_PORT) > LIGHT_THRESHOLD )
 *              respond_to_light();
 *
 *          // The remote control unit sends new data periodically
 *          // (every 18.5 ms for the crystal-based remote control unit)
 *          if ( rc_new_data_available() )
 *          {
 *              rc_routine();
 *              if ( condition1 )
 *                  autonomous_routine1();
 *              if ( condition2 )
 *                  autonomous_routine2();
 *          }
 *      }
 *  }
 *  \endcode
 *
 *  The program above begins in remote controlled (RC) mode.
 *  The main loop reads sensor values, and checks for new data from
 *  the RC unit, which is sent at fixed intervals.  When new RC data
 *  arrives, the rc_routine() function is called to process
 *  information from the remote control unit.  The rc_routine() function
 *  can also check sensors if necessary.  Based on inputs from the RC unit
 *  and sensors, the rc_routine() function controls the motors and servos.
 *
 *  The robot can also operate in autonomous mode, where it is
 *  in complete control of itself, and ignores inputs from the RC
 *  unit.  The robot makes its own decisions based solely on sensor input
 *  and/or a predefined sequence of actions.
 *
 *  It is highly recommended that you always begin your programs in RC
 *  mode, and use some sort of input to trigger
 *  autonomous routines.  The Vex controller begins executing a new
 *  program as soon as it is uploaded to the
 *  controller.  Hence, if your robot runs the autonomous routine
 *  immediately, it could take off across the room 
 *  with the programming cable still
 *  attached.  The conditions for triggering the autonomous routines
 *  are up to you.
 *  The RC routine or main program could watch for a particular button
 *  or joystick movement on the RC unit, or a sensor such as a push button.
 *
 *  This documentation is limited to functions provided by the OpenVex
 *  library.  Many more functions are available in the libraries provided
 *  by SDCC (http://sdcc.sourceforge.net/doc/sdccman.html/sdccman.html)
 *  and MCC18 (http://www.microchip.com/stellent/idcplg?IdcService=SS_GET_PAGE&nodeId=1406&dDocName=en010014).
 *
 *  While OpenVex is compatible with both SDCC and MCC18, the libraries
 *  provided with the two compilers may differ significantly.  Hence,
 *  use of any library functions not provided by OpenVex may impact the
 *  portability of your code between the two compilers.  Check the
 *  documentation of both compilers to see which functions are compatible
 *  with both.
 */

#include <stdio.h>
#include <usart.h>
#include "general.h"
#include "platform.h"
#include "vex_usart.h"
#include "io.h"
#include "timer.h"
#include "interrupts.h"
#include "shaft_encoder.h"
#include "vex_spi.h"
#include "master.h"
#include "sonar.h"
#include "debug.h"
#include "init.h"
#include "vex_delay.h"
#include "lvd.h"
#include "accelerometer.h"
#include "line_sensor.h"
#include "arcade_drive.h"

/* Pointer to one of the double buffers controlled by the master SPI ISR */
/* Essential global variables defined in the libraries */
extern volatile rx_data_t       *User_rxdata;
extern tx_data_t                User_txdata;
extern volatile tx_data_t       Tx_buff[2];
extern volatile unsigned char   Tx_user_buff_index;

