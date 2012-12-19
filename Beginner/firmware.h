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

#ifndef __firmware_h__
#define __firmware_h_

/* 
 *  Place jumper in I/O port to enable competition mode.  Robot will
 *  enter autonomous_routine_competition() as soon as an RC signal is
 *  received after power-on, and switch to user-controlled mode
 *  when the routine returns.
 */
#define COMPETITION_JUMPER_PORT 15

/* Place a jumper in I/O port for arcade drive. No jumper = tank drive. */
#define ARCADE_JUMPER_PORT      16
#define AUTONOMOUS_TRIGGER_CHAN 5   /* RC Button */

/*
 *  The #defines below are the guide to configuring your robot.
 */

/* How many of the 16 I/O ports should be configured for analog input */
#define ANALOG_PORTS            4

/* Drive motors */
#define LEFT_DRIVE_PORT         3
#define RIGHT_DRIVE_PORT        4
#define IMPELLER_PORT           5
#define ARM_PORT                6

#define TANK_DRIVE_LEFT_CHAN    3
#define TANK_DRIVE_RIGHT_CHAN   2
#define TANK_IMPELLER_CHAN      5
#define TANK_ARM_CHAN           6

#define ARCADE_DRIVE_X_CHAN     4
#define ARCADE_DRIVE_Y_CHAN     3
#define ARCADE_IMPELLER_CHAN    2
#define ARCADE_ARM_CHAN         1

#define AUTON_DRIVE_SPEED       70

/* General stuff */
#ifndef MIN
#define MIN(x,y)    ((x) < (y) ? (x) : (y))
#endif

#ifndef MAX
#define MAX(x,y)    ((x) > (y) ? (x) : (y))
#endif

#ifndef ABS
#define ABS(x)      ((x) >= 0 ? (x) : -(x))
#endif

#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE   0
#endif

/* Prototypes */
/* ifi_startup.c */
void _entry(void);
void _startup(void);
void _do_cinit(void);
/* firmware.c */
void main(void);
void custom_init(void);
void rc_routine(void);
void vex_default_routine(void);
void tank_drive_routine(void);
void autonomous_routine1(void);
void autonomous_routine0(void);
void sonar_scan(unsigned char port);
void arcade_drive_routine(void);
void    autonomous_routine_competition(unsigned short seconds);
#endif  /* __firmware_h__ */

