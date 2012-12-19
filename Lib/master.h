/**************************************************************************
* Description: 
*   Macros, typedefs, and prototypes for communication with the master
*   microprocessor.
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
***************************************************************************
*   History:
*       Created Dec 2008    Jason W. Bacon
***************************************************************************/

#ifndef __master_h__
#define __master_h__

#define NO_WAIT 0
#define WAIT    1

/* Buttons always appear to send 0, 127, or 255, so why the threshold? */
#define BUTTON_REV_THRESH       -27
#define BUTTON_FWD_THRESH       +27

/* Convenience macros for motor control */
#define MOTOR_STOP              (char)0
#define MOTOR_FULL_FWD          (char)127
#define MOTOR_FULL_REV          (char)-127

#define SERVO_CENTER            (char)0
#define SERVO_FULL_LEFT         (char)-127
#define SERVO_FULL_RIGHT        (char)+127

#define JOY_CENTER              (char)0
#define JOY_FULL_LEFT           (char)-127
#define JOY_FULL_RIGHT          (char)+127
#define JOY_FULL_DOWN           (char)-127
#define JOY_FULL_UP             (char)+127

/* Masks for master_set_user_command() */
#define TX_CMD_AUTONOMOUS_MODE  0x02    /* Set this bit to begin autonomous */

/* Tell master not to send SPI interrupts. User processor runs solo. */
#define TX_CMD_BYTE1_SPI_OFF    0x10

/**
 *  Set the new data flag.  This flag is set by the SPI interrupt handler
 *  and must be cleared by calling MASTER_CLR_NEW_RC_DATA_FLAG() to prevent
 *  a false indication of new data.
 */

#define MASTER_SET_NEW_RC_DATA_FLAG(s) ((s).new_rc_data = 1)

/**
 *  Clear the new data flag.  This flag is set by the SPI interrupt handler
 *  and must be cleared by calling this function to prevent a false
 *  indication of new data.
 */

#define MASTER_CLR_NEW_RC_DATA_FLAG(s) ((s).new_rc_data = 0)

typedef enum
{
    PWM_USER,
    PWM_MASTER
}   pwm_controller_t;

/* master.c */
char rc_read_status(void);
char rc_read_data(unsigned char channel);
char controller_in_autonomous_mode(void);
char rc_new_data_available(void);
unsigned char rc_button_sequence(unsigned char, unsigned char);
char pwm_write(unsigned char port, char val);
char pwm_read(unsigned char port);
void pwm_select_processor(unsigned char port, pwm_controller_t controller);
void pwm_set_output_type(int pwmSpec1, int pwmSpec2, int pwmSpec3, int pwmSpec4);
void controller_submit_data(unsigned char wait);
void master_set_user_cmd(unsigned char cmd);
void master_clr_user_cmd(unsigned char cmd);
void controller_begin_autonomous_mode(void);
void controller_end_autonomous_mode(void);
void controller_print_version(void);

#endif

