/**************************************************************************
*  
*   Functions for communicating with the master processor.
*   In the VEX, the master processor collects data from the remote
*   control unit, and generates PWM outputs to drive the motors and
*   servos.  This frees the user processor to focus on the I/O
*   and calculations necessary to operate the robot.
*   The master sends and interrupt signal to the user processor every
*   18.5ms, at which time the program on the user processor should
*   retrieve new data from the SPI buffer.
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

#include <stdio.h>
#include <string.h>
#include "platform.h"
#include "vex_usart.h"
#include "io.h"
#include "vex_spi.h"
#include "general.h"
#include "master.h"

unsigned char                   Pwm_disable_mask;

/************************************************************************
 *  rx_data_t functions
 *  These functions read data from the rxdata structure, which is
 *  read from the master processor over an SPI bus using Getdata().
 ************************************************************************/

/**
 *  \defgroup RC Remote Control
 *  @{
 */
 
/**
 *  Determine current state of remote control unit.
 *
 *  \returns    1 if the remote control is on, 0 if off, 4 if disabled
 *              (controller is in autonomous mode).
 */

/*
 * History:
 *  Dec 2008     J Bacon
 */

char    rc_read_status(void)

{
    return User_rxdata->rc_status_byte;
}


/**
 *  Retrieve most recently sampled data from the specified channel
 *  on the remote control unit.  All values returned range from -127 to +127.
 *  For joysticks, 0 is the center positionm, full left or full up is -127,
 *  and full down or full right is +127.
 *  For buttons, 0 indicates that neither button for the channel is
 *  pressed, -127 indicates the lower button, and 127 indicates the upper.
 *
 *  \param  channel Channel (joystick axis or button pair) to read.
 *  
 *  \returns    Value read from channel (-127 to +127)
 */

/*
 * History:
 *  Dec 2008     J Bacon
 */

char    rc_read_data(unsigned char channel)
{
    /* RC_CHANNEL* identifiers are aliases to global PORT*bits fields */
    if ( VALID_RC_CHANNEL(channel) )
    {
	/* 
	 *  Master returns 0 for full left/down, 127 for center, 255 for
	 *  full right/up.  We want 0 to be center, so we subtract 127.
	 *  255 is a special case, since 255 - 127 = +128, which is
	 *  interpreted as -128 in signed char format.  This is at the
	 *  wrong end of the spectrum, so we simply eliminate this
	 *  rogue value.  Had the designers chosen 128 as the center
	 *  position, we would not have this issue.
	 */
	if ( User_rxdata->oi_analog[channel-1] == 255 )
	    --User_rxdata->oi_analog[channel-1];
	return User_rxdata->oi_analog[channel-1] - 127;
    }
    else
	return OV_BAD_PARAM;
}


/**
 *  Return a value indicating a binary sequence on RC button 'channel'.
 *  This function allows a single button pair to trigger any number of
 *  different functions.
 *
 *  This function keeps a static internal value and counter.
 *  Each time this function is called, the buttons on channel are
 *  sampled.  If the lower button is pressed, a 0 is inserted into least
 *  significant bit of the value, and the counter incremented.  If the
 *  upper button is pressed, a 1 is inserted and the counter is incremented.
 *  If neither button is pressed, neither the value nor counter are changed.
 *
 *  A button being held down will only be counted once.  It must be released
 *  and pressed again to advance the sequence.
 *
 *  When the counter reaches 'len', the function returns the value
 *  based on the binary sequence of button presses.  For example, if
 *  the sequence length is 3, and the user presses the sequence
 *  lower, upper, upper, then the value returned is 011 binary
 *  (3 in decimal).
 *
 *  Until then, it returns -1, indicating that the sequence is not
 *  yet complete.
 *
 *  \param  channel Channel of button pair to read.
 *  \param  len     Length of button sequence.
 *
 *  \returns    -1 if sequence is not complete, or value of binary sequence.
 */

unsigned char   rc_button_sequence(unsigned char channel, unsigned char len)

{
    static unsigned char    count = 0,
			    val = 0,
			    temp,
			    digit;
    static char             button,
			    old_button;

    if ( ! VALID_RC_CHANNEL(channel) )
	return OV_BAD_PARAM;

    switch ( (button = rc_read_data(channel)) )
    {
	case    -127:
	    digit = 0;
	    break;
	case    +127:
	    digit = 1;
	    break;
	default:
	    digit = 2;
	    break;
    }
    
    /*
     *  Register only distinct button pressed.  This function could be
     *  called thousands of times while a button is being held down,
     *  but this does not constitute a button sequence.
     */
    if ( (digit < 2) && (button != old_button) )
    {
	++count;
	val = (val << 1) + digit;     /* Insert new digit in LSB */
    }
    old_button = button;
    if ( count == len )
    {
	temp = val;
	count = val = 0;
	return temp;
    }
    else
	return OV_SEQUENCE_INCOMPLETE;
}


/**
 *  Determine whether new data is available from the remote control unit.
 *
 *  \returns    TRUE when new data is available, FALSE if not.
 */

/****************************************************************************
 *  This indicator is normally set by the SPI interrupt handler, and
 *  is reset by rc_new_data_available().
 *
 * History: 
 *  Dec 2008    J Bacon     Derived from Vex default code.
 *  May 2009    J Bacon     Rewrote double-buffer mechanism to eliminate
 *                          mem copy to thrid buffer.
 ***************************************************************************/

char   rc_new_data_available(void)

{
    unsigned char   new_rc_data;
    
    new_rc_data = Spi_status.new_rc_data;
    if ( new_rc_data )
    {
	/*
	 *  Tell SPI ISR that we've checked the current buffer.
	 *  Make sure this isn't interrupted by the SPI ISR!
	 */
	PIE1bits.SSPIE = 0;
	MASTER_CLR_NEW_RC_DATA_FLAG(Spi_status);
	PIE1bits.SSPIE = 1;

	// Pwm_disable_mask will be 0 unless USER_CPP is in use
	// for some ports.
	// if (Pwm_disable_mask != 0x0f)
	//    io_update_local_pwm_dir(User_txdata.pwm_mask);
    }
    return new_rc_data;
}

/** @} */


/****************************************************************************
 *  These functions deal with the SPI bus connecting the master and
 *  user processors.
 ***************************************************************************/ 

/**
 *  \defgroup controller Controller Status
 *  @{
 */
 
/**
 *  Determine current operation mode of the controller.
 *
 *  \returns    TRUE if the controller is in autonomous mode, FALSE if not.
 */

/*
 * History:
 *  Dec 2008     J Bacon
 */

char    controller_in_autonomous_mode(void)

{
    return User_rxdata->rc_mode.autonomous;
}


/**
 *  Signal master processor to end autonomous mode and return to
 *  remote control mode.
 */

/***************************************************************************
 * History: 
 *  Aug 2009    J Bacon
 ***************************************************************************/

void    controller_end_autonomous_mode(void)

{
    /* Switch master processor back to RC mode */
    master_clr_user_cmd(TX_CMD_AUTONOMOUS_MODE);
    controller_submit_data(WAIT);
    
    /*
     *  Make sure master processor is out of autonomous mode before
     *  returning so we don't end up back here by mistake.
     */
    while ( controller_in_autonomous_mode() )
	;
}


/**
 *  Signal controller to begin autonomous mode.  In autonomous
 *  mode, the controller ignores RC input and allows the user program
 *  to operate without checking for it.
 *
 *  Note: The orange light
 *  on the Vex controller blinks more rapidly in this mode than in
 *  remote control (RC) mode.
 */

/***************************************************************************
 * History: 
 *  Aug 2009    J Bacon
 ***************************************************************************/

void    controller_begin_autonomous_mode(void)

{
    /* Signal master processor to enter autonomous mode */
    master_set_user_cmd(TX_CMD_AUTONOMOUS_MODE);
    controller_submit_data(WAIT);
    
    /* Don't return until switch to autonomous is complete */
    while ( ! controller_in_autonomous_mode() )
	;
}


/**
 *  Buffer PWM data, etc. to be sent to the motors/servos.
 *
 *  This function must be called following other controller_*()
 *  and pwm_*() functions in order to make the updates take effect.
 *  This allows
 *  multiple PWMs to be altered using separate pwm_write() calls
 *  without actually changing the motor speeds at separate times.
 *  Instead, pwm_write() places changes in the buffer, and all
 *  changes are submitted together by controller_submit_data(), so that
 *  motor speeds can all be changed (almost) simultaneously.
 *
 *  This can also reduce load on the PIC user processor caused by
 *  buffer management for each packet sent.  Programmers are
 *  encouraged to make multiple changes via pwm_write() and
 *  controller_*() functions before each expensive controller_submit_data()
 *  call.
 *
 *  \param wait One of the manifest constants WAIT or NO_WAIT.
 *              Specifies whether or not to wait for data to be submitted
 *              before returning to the caller.
 *
 *  PIC controllers:
 *
 *  If WAIT is used, controller_submit_data() will not return until the master
 *  processor has picked up the packet.  This could mean up to a 18.5 ms
 *  delay before returning to the caller, during which no other code
 *  (except interrupt handlers) can execute.  Generally, this should only
 *  be done if the next statement after controller_submit_data() should
 *  not be executed until the master processor has received the current
 *  command.
 *
 *  If NO_WAIT, controller_submit_data() returns immediately, and the data
 *  will be picked up by the master processor at an unknown time within
 *  the next 18.5 ms.  This is the preferred method if it isn't critical
 *  when the submitted data takes effect, since it avoids burning up to
 *  18.5 ms of CPU time that could be used for useful work.
 *
 *  Example: If you want a delay loop to begin after motors have actually
 *  started, to guarantee that the motors run for the full specified delay
 *  period:
 *
 *  \code
 *  pwm_write(LEFT_DRIVE_PORT, 50);
 *  pwm_write(RIGHT_DRIVE_PORT, -50);
 *  controller_submit_data(WAIT);
 *  delay_msec(250); 
 *  \endcode
 *
 *  Note that in many applications, it wouldn't matter much if the
 *  motors run for 231.5 ms instead of 250 ms, so NO_WAIT may be
 *  perfectly reasonable in the above example.
 */


/****************************************************************************
 *  Buffer txdata for transmission to the master processor.
 *
 * History: 
 *  Dec 2008    J Bacon     Derived from Vex default code.
 ***************************************************************************/

void    controller_submit_data(unsigned char wait)

{
    /* Only do this if the ISR has sent the previous copy */
    /* On second thought, is this a good idea?  This will prevent
       old, unsent data from being replaced by newer data. Hence,
       new data will be lost. It also means calls to submit_data
       don't always submit the data, which is a bad API. */
    //if (!Spi_status.new_tx_data)
    {
	/* If not the first packet, run some checks. */
	if (User_txdata.current_mode == 2)
	    check_tx_env();

	/* 
	 *  Clear bit 7 for what reason I do not know.  Doesn't actually
	 *  seem to matter, but it's in the default code.
	 */
	User_txdata.cmd_byte1 &= 0x7F;
	
	/*
	 *  Copy txdata to the SPI ISR's double buffer.
	 */
	memcpy(Tx_buff + Tx_user_buff_index,&User_txdata,sizeof(tx_data_t));
	
	/*
	 *  Toggle double buffers and indicate new data is ready.
	 *  Make sure the following is not interrupted by the SPI ISR,
	 *  since it modifies these same variables!
	 */
	PIE1bits.SSPIE = 0;
	Tx_user_buff_index = !Tx_user_buff_index;
	Spi_status.new_tx_data = 1;
	PIE1bits.SSPIE = 1;
	
	/* Tell master this is a new packet? */
	User_txdata.packet_num++;
    }
    
    /* Wait for master to receive new data */
    if ( wait == WAIT )
	while ( !rc_new_data_available() )
	    ;
}


/**
 *  Print the firmware version(s) currently on the controller.
 */


void    controller_print_version(void)

{
    printf("\nVEX master code v%d\n%s\n",
	(int)User_rxdata->master_version, CODE_VERSION);
}

/** @} */


/************************************************************************
 *  tx_data_t functions
 *  These functions set up the txdata structure, which the program must
 *  send to the master controller over an SPI bus using Putdata().
 ************************************************************************/

/**
 *  \defgroup pwm Motor and Servo Control
 *  @{
 */
 
/**
 *  Write a PWM value to the specified port.
 *  
 *  \param  port    PWM port to write.
 *  \param  val     PWM power/position values from -127 to 127.
 *
 *  \returns    OV_OK on success, OV_BAD_PARAM if the port or
 *              PWM value are invalid.
 *
 *  For motors, 0 means stop.  For servos, 0 is the center position.
 *
 *  Changes made through this function will not take effect until
 *  controller_submit_data() is called.
 *
 *  On PIC-based controllers:
 *
 *  It will then take between 0 and 18.5 ms before the submitted data
 *  reaches the master processor which controls the motors.
 *
 *  To alter a motor/servo setting, the following must occur:
 *
 *      1. call pwm_write().  This only alters the next packet to be submitted.\n
 *      2. call controller_submit_data().  This queues the altered packet for
 *      sending to the master processor.\n
 *      3. The master processor interrupts the user processor every
 *      18.5 ms to send sample data from the RC unit and retrieve
 *      the latest data queued by controller_submit_data().
 *
 *      Hence, motor setting will change between 0 and 18.5 ms after
 *      your code calls controller_submit_data().
 */

/*
 * History:
 *  Dec 2008     J Bacon
 */

char    pwm_write(unsigned char port,char val)
{
    /*
     *  Master processor treats 127 as stop/center, so we add 127 to
     *  map -127 to 0 (full reverse), 0 to 127 (stop/center), 
     *  127 to 254 (full forward).  Adding -128 + 127
     *  yields 0xff, which 255 in unsigned, and at the wrong end of
     *  the power spectrum.  To eliminate this discontinuity, we
     *  simply eliminate this one rogue value.  Had the designers
     *  made 128 the stop/center value, we would not have this issue.
     */
    if ( val == -128 )
	++val;
    /* No need to validate val, since it's range limits it to +/-127 */
    if ( VALID_PWM_PORT(port) )
    {
	User_txdata.pwm[port-1] = val + 127;
	return OV_OK;
    }
    else
	return OV_BAD_PARAM;
}


/**
 *  Read the PWM value for the specified port from the txdata structure.
 *  
 *  \param  port    PWM port to read back.
 *
 *  \returns    Last value written to port using pwm_write().
 *
 *  NOTE: This does not necessarily indicate the current power setting on
 *        the motor/servo.  It simply reports the latest value written to
 *        this port via pwm_write().  See pwm_write() for details.
 */

/*
 * History:
 *  Dec 2008     J Bacon
 */

char    pwm_read(unsigned char port)
{
    if ( VALID_PWM_PORT(port) )
	return User_txdata.pwm[port-1] - 127;
    else
	return OV_BAD_PARAM;
}


/*
 *  Designate which controller will control the specified PWM port.
 *  Valid arguments are PWM_MASTER and PWM_USER.  Currently, only
 *  PWM_MASTER is supported.  Changes made through this function
 *  will not take effect until controller_submit_data() is called.
 */

/*
 * History:
 *  Dec 2008     J Bacon
 */

void    pwm_select_processor(unsigned char port,
	    pwm_controller_t controller)
{
    switch(controller)
    {
	case    PWM_MASTER:
	    User_txdata.pwm_mask |= (1 << ((port)-1));
	    break;
	case    PWM_USER:
	    User_txdata.pwm_mask &= ~(1 << ((port)-1));
	    break;
    }
}


/*
 *  Select output mode for CCP ports (motor ports 1-4).  Valid arguments
 *  for each port are IFI_PWM for standard PWM output and USER_CCP
 *  to use the port for digital I/O or CCP.  Currently, only IFI_PWM
 *  is supported.
 */

/*
 * History: 
 *  Dec 2008    J Bacon     Derived from Vex default code.
 */

void    pwm_set_output_type(int pwmSpec1, int pwmSpec2, int pwmSpec3, int pwmSpec4)

{
    Pwm_disable_mask = 0;

    if (pwmSpec1 == USER_CCP)
	Pwm_disable_mask |= 0x01;
    else
	CCP2CON = 0;

    if (pwmSpec2 == USER_CCP)
	Pwm_disable_mask |= 0x02;
    else
	CCP3CON = 0;

    if (pwmSpec3 == USER_CCP)
	Pwm_disable_mask |= 0x04;
    else
	CCP4CON = 0;

    if (pwmSpec4 == USER_CCP)
	Pwm_disable_mask |= 0x08;
    else
	CCP5CON = 0;
}

/** @} */


/*
 *  Set bits in the user command to be sent to the master processor.
 *  The update will not be transmitted
 *  to the master processor until controller_submit_data() is called.
 *  See master.h for possible values.
 */

/*
 * History:
 *  Dec 2008     J Bacon
 */

void    master_set_user_cmd(unsigned char cmd)

{
    User_txdata.user_cmd |= cmd;
}


/*
 *  Clear bits in the user command to be sent to the master processor.
 *  The update will not be transmitted
 *  to the master processor until controller_submit_data() is called.
 *  See master.h for possible values.
 */

void    master_clr_user_cmd(unsigned char cmd)

{
    User_txdata.user_cmd &= ~cmd;
}

