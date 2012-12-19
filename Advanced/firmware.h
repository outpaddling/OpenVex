/**************************************************************************
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
*
***************************************************************************/

#ifndef __sample_code_h__
#define __sample_code_h_

/* Macros and manifest constants */
#define ENABLE_AUTONOMOUS_ROUTINES  1

#define DEFAULT_CODE    0
#define TANK            1
#define ARCADE          2

/* Enable exactly one of the three below */
//#define DRIVE_ROUTINE           DEFAULT_CODE
//#define DRIVE_ROUTINE           TANK
#define DRIVE_ROUTINE           ARCADE

/* 
 *  Place jumper in I/O port to enable competition mode.  Robot will
 *  enter autonomous_routine_competition() as soon as an RC signal is
 *  received after power-on, and switch to user-controlled mode
 *  when the routine returns.
 */
#define COMPETITION_JUMPER_PORT 15

/*
 *  The #defines below are the guide to configuring your robot.
 */

/* How many of the 16 I/O ports should be configured for analog input */
#define ANALOG_PORTS            8

/* Line follower */
#define LINE_SENSOR_LEFT_PORT   1
#define LINE_SENSOR_CENTER_PORT 2
#define LINE_SENSOR_RIGHT_PORT  3

/* Accelerometer */
#define ACCEL_X_PORT            4
#define ACCEL_Y_PORT            5
#define ACCEL_Z_PORT            6

/* Light sensor */
#define LIGHT_SENSOR_PORT       7

/* Front bumper switches */
#define BUMPER_LEFT_PORT        10
#define BUMPER_RIGHT_PORT       11

/* Sonar */
#define SONAR_OUTPUT_PORT       9
#define SONAR_INTERRUPT_PORT    3
#define SONAR_SERVO_PORT        2
#define SONAR_MAX_ANGLE         70

/* Drive motors */
#define LEFT_DRIVE_PORT         3
#define RIGHT_DRIVE_PORT        4

#define LEFT_TANK_DRIVE_CHAN    3
#define RIGHT_TANK_DRIVE_CHAN   2

#define ARCADE_DRIVE_X_CHAN     4
#define ARCADE_DRIVE_Y_CHAN     3
#define ARCADE_IMPLEMENT_CHAN   2

/*
 *  Use interrupt ports 1 and 2 if possible.  Each of them has a dedicated
 *  interrupt channel, so they are processed more efficiently.  Ports 3 - 6
 *  share a single interrupt channel, so the ISR has to check all of them
 *  every time an interrupt occurs.
 *  The likelihood of missing an input due to shared interrupts
 *  on ports 3 - 6 is extremely remote, so this is not a big issue.  It's
 *  just good habit to use the best options available.
 */
#define LEFT_ENCODER_INTERRUPT_PORT     1
#define RIGHT_ENCODER_INTERRUPT_PORT    2

/* Implement */
#define IMPLEMENT_CHAN          6
#define IMPLEMENT_SERVO_PORT    1
#define JOY_THRESHOLD           20
#define IMPLEMENT_UP            -100
#define IMPLEMENT_DOWN          18
#define IMPLEMENT_GEAR_DOWN     3

/* autonomous routine triggers */
#define AUTO_TRIGGER_BUTTON     5
#define AUTO_TRIGGER_SEQ_LEN    2

/* autonomous_routine0() */
#define TURN_TIME               1200
#define FORWARD_TICKS           100
#define AUTON_POWER             50

/* autonomous_routine1() */
#define MOTOR_POWER1            50
#define ROUTINE1_DURATION       20
#define ROUTINE1_STOP_BUTTON    5
#define ROUTINE1_SONAR_DISTANCE 50
#define ROUTINE1_BACKUP_MS      1000
#define ROUTINE1_SONAR_SERVO_DELAY  40
#define ROUTINE1_SPIN_MS        500

/* autonomous_routine2() */
#define ROUTINE2_POWER          80
#define ROUTINE2_TICKS          600

/* Prototypes */
/* ifi_startup.c */
void _entry(void);
void _startup(void);
void _do_cinit(void);
/* sample_code.c */
void main(void);
void custom_init(void);
void rc_routine(void);
void vex_default_routine(void);
void tank_drive_routine(void);
void autonomous_routine3(void);
void autonomous_routine2(void);
void autonomous_routine1(void);
void autonomous_routine0(void);
void sonar_scan(unsigned char port);
void arcade_drive_routine(void);
void auton_msg(const char *str, const char *name);
void autonomous_routine_competition(unsigned short seconds);
#endif  /* __sample_code_h__ */

