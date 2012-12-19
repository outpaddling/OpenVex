/**************************************************************************
*   
*       Digital/Analog IO functions.
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

/**
 *  \defgroup IO Analog and Digital I/O
 *  @{
 *
 *  These functions provide basic input, output, and configuration
 *  for digital and analog I/O ports.
 */

#include <stdio.h>
#include "platform.h"
#include "general.h"
#include "vex_usart.h"
#include "io.h"

unsigned char   Analog_ports,
		Analog_ports_const;

/**
 *  Set the number of analog ports.  On the 18F8520, we cannot arbitrarily
 *  set individual ports for analog or digital operation.  For any value of
 *  ports between 1 and 14, or 16, ports 1 through ports are set for
 *  analog operation and the rest are set for digital.
 *  It is not possible to set the controller for 15
 *  analog ports.  The number of analog/digital ports is specified by 
 *  4 bits in hardware, which means there are 16 possible values.
 *  There are 17 possible numbers of analog ports (0 to 16 inclusive).
 *  The PIC hardware designers chose to support values of
 *  0 through 14 and 16
 *  analog ports.  Hence, these are the allowed values for ports.
 *  A value of 15 and any value greater than 16 is ignored.
 *
 *  \param      ports   The number of I/O ports to use for analog input.
 *
 *  \returns    OV_OK on success, OV_BAD_PARAM for invalid port count.
 */
 
/*
 * History:
 *  Dec 2008    J Bacon
 *  Jan 2008    J Bacon     Eliminated switch and ADC_xANA constants
 *                          to reduce function size.
 */

status_t    io_set_analog_port_count(unsigned char ports)

{
    /*
     *  The bit pattern used by the 18F8520 ADC represents the number of
     *  digital ports if 0000, or the number of digital ports + 1
     *  if > 0000.  MCC18 may use ORable or ANDable masks.  ADC_MASK
     *  is used to compensate.  ( See ADC_V2 in adc.h )
     */
     
    // FIXME: PIC-dependent
    if ( ports <= 14 )
    {
	Analog_ports = ports;
	Analog_ports_const = (0x0F - ports) | ADC_MASK;
	return OV_OK;
    }
    else if ( ports == 16 )
    {
	Analog_ports = ports;
	Analog_ports_const = 0x00 | ADC_MASK;
	return OV_OK;
    }
    else
	return OV_BAD_PARAM;
}


/**
 *  Returns the current number of I/O ports configured for analog input.
 *  See io_set_analog_port_count() for more information.
 *
 *  \returns    The number of analog ports currently configured.
 */ 

unsigned char   io_get_analog_port_count(void)

{
    return Analog_ports;
}


/***************************************************************************
 * Description:
 *  adc_open with SDCC 2.8 doesn't work with the VEX (pic16f8520).
 *  This is a stop-gap provided until the problem is corrected.
 *
 * Arguments:
 *  channel:    I/O port from 0 - 15
 *
 * History: 
 *  Dec 2008    J Bacon
 ***************************************************************************/

#ifdef SDCC
void    adc_open8520(unsigned char channel)

{
    /* Clean slate. */
    ADCON0 = 0;
    
    /* Set channel (port) in bits 5-2. */
    ADCON0 |= (channel << 2);

    /* Halt any in-progress conversions until registers configured. */
    ADCON0bits.GO = 0;
    
    /* Clean slate. */
    ADCON1 = 0;
    
    /* Use the default internal voltage references for both + and - */
    ADCON1bits.VCFG1 = 0;
    ADCON1bits.VCFG0 = 0;
    
    /* Tell ADC how many of the ports are analog. */
    ADCON1 |= Analog_ports_const;
    
    /* Determine where the 10 bits go within the 16 bit result. */
    ADCON2 |= ADC_FRM_RJUST;
    
    /* Use A/D's internal RC oscillator. */
    ADCON2bits.ADCS2 = 1;
    ADCON2bits.ADCS1 = 1;
    ADCON2bits.ADCS0 = 1;

    /* Disable A/D interrupts. */
    PIR1bits.ADIF = ADC_INT_OFF;
    PIE1bits.ADIE = 0;
    
    /* Enable the A/D module. */
    ADCON0bits.ADON = 1;
}

/*
 *  The 2.8 library versions of adc_conv() and adc_busy() don't work with
 *  the Vex for some reason, even though adc_conv() uses the exact same
 *  code you see below.  Is the lib binary incompatible with the 18f8520?
 *  The assembly code in the lib adc_busy() doesn't work if compiled here
 *  either.
 *  The library adc_read() works fine.
 */
 
void adc_conv(void)
{
    ADCON0bits.GO = 1;
}

char    adc_busy(void) __naked
{
    return ADCON0bits.GO;
}
#endif

/**
 *  Read analog input from I/O port "port".  The port must be among those
 *  configured for analog input (see io_set_analog_port_count()).
 *  The analog value returned
 *  contains 10 significant bits of data, and ranges from 0x000 to 0x3ff.
 *  The meaning of the value returned is dependent on the sensor
 *  connected to the port.  See the sensor documentation, or figure
 *  it out by trial and error.
 *
 *  \param      port    The analog port to be read.
 *
 *  \returns    Analog value from 1 to 0x3ff on success,
 *              OV_BAD_PARAM if the specified port is not valid
 *              or not configured for analog input.
 */

/*
 * History:
 *  Dec 2008     J Bacon
 */

unsigned int io_read_analog(unsigned char port)

{
    unsigned int    result;
    unsigned char   channel;
#ifndef SDCC
    static unsigned char inputs[] = {
	ADC_CH0, ADC_CH1, ADC_CH2, ADC_CH3,
	ADC_CH4, ADC_CH5, ADC_CH6, ADC_CH7,
	ADC_CH8, ADC_CH9, ADC_CH10, ADC_CH11,
	ADC_CH12, ADC_CH13, ADC_CH14, ADC_CH15 };
#endif

    /* Make sure port does not exceed current Analog_ports */
    if ( ! VALID_ANALOG_PORT(port) )
	return OV_BAD_PARAM;

#ifdef SDCC
    result = 0;

    channel = port - 1;

    /*
     *  SDCC generic adc_open() doesn't work for the 8520.  Lots of
     *  stuff missing.
     */
    adc_open8520(channel);

    /* Allow settling time before starting a conversion */
    delay10tcy(10);
    
    adc_conv();
    while ( adc_busy() )
	;
    result = adc_read();
    adc_close();

#else   /* MCC18 */

    channel = inputs[port - 1];

    OpenADC(ADC_FOSC_RC & ADC_RIGHT_JUST & Analog_ports_const,
	  channel & ADC_INT_OFF & ADC_VREFPLUS_VDD & ADC_VREFMINUS_VSS);
    Delay10TCYx(10);
    ConvertADC();
    while (BusyADC())
	;
    ReadADC();
    CloseADC();
    result = (unsigned int)ADRESH << 8 | ADRESL;
#endif

    return result;
}

/**
 *  Return the sampled value (0 or 1) from port. The port must be among those
 *  configured for digital input (see io_set_analog_port_count())
 *  and the port must have been configured for
 *  output using io_set_port_direction().
 *
 *  \param      port    The digital port to be read.
 *
 *  \returns    0 or 1 on success, OV_BAD_PARAM if the port is invalid or
 *              not configured for digital input.
 */

/*
 * History:
 *  Dec 2008     J Bacon
 */

char    io_read_digital(unsigned char port)

{
    if ( ! VALID_DIGITAL_INPUT_PORT(port) )
	return OV_BAD_PARAM;
    
    /* DIGITAL_IN* identifiers are aliases to global PORT*bits fields */
    switch(port)
    {
	case    1:  return DIGITAL_IN1;
	case    2:  return DIGITAL_IN2;
	case    3:  return DIGITAL_IN3;
	case    4:  return DIGITAL_IN4;
	case    5:  return DIGITAL_IN5;
	case    6:  return DIGITAL_IN6;
	case    7:  return DIGITAL_IN7;
	case    8:  return DIGITAL_IN8;
	case    9:  return DIGITAL_IN9;
	case    10: return DIGITAL_IN10;
	case    11: return DIGITAL_IN11;
	case    12: return DIGITAL_IN12;
	case    13: return DIGITAL_IN13;
	case    14: return DIGITAL_IN14;
	case    15: return DIGITAL_IN15;
	case    16: return DIGITAL_IN16;
	default:
	    return OV_BAD_PARAM;
    }
}


/**
 *  Set the output value of "port".  Port must be among those configured
 *  for digital I/O (see io_set_analog_port_count()) and must have been
 *  previously configured for digital output using io_set_direction().
 *  The argument "val" must be 0 or 1.
 *
 *  \param      port    Digital port to write to.
 *  \param      val     Value of 0 or 1 to write to port.
 *
 *  \returns    OV_OK on success, OV_BAD_PARAM if port is invalid or
 *              not configured for digital output.
 */

/*
 * History:
 *  Dec 2008     J Bacon
 */

status_t    io_write_digital(unsigned char port,unsigned char val)
{
    if ( ! VALID_DIGITAL_OUTPUT_PORT(port) )
	return OV_BAD_PARAM;

    /* DIGITAL_OUT* identifiers are aliases to global PORT*bits fields */
    /* FIXME: Check IO_DIRECTION */
    switch(port)
    {
	case    1:  DIGITAL_OUT1 = val; break;
	case    2:  DIGITAL_OUT2 = val; break;
	case    3:  DIGITAL_OUT3 = val; break;
	case    4:  DIGITAL_OUT4 = val; break;
	case    5:  DIGITAL_OUT5 = val; break;
	case    6:  DIGITAL_OUT6 = val; break;
	case    7:  DIGITAL_OUT7 = val; break;
	case    8:  DIGITAL_OUT8 = val; break;
	case    9:  DIGITAL_OUT9 = val; break;
	case    10:  DIGITAL_OUT10 = val; break;
	case    11:  DIGITAL_OUT11 = val; break;
	case    12:  DIGITAL_OUT12 = val; break;
	case    13:  DIGITAL_OUT13 = val; break;
	case    14:  DIGITAL_OUT14 = val; break;
	case    15:  DIGITAL_OUT15 = val; break;
	case    16:  DIGITAL_OUT16 = val; break;
	default:
	    return OV_BAD_PARAM;
    }
    return OV_OK;
}


/**
 *  Select the direction (IO_DIRECTION_IN or IO_DIRECTION_OUT) for the given port.
 *  The port must be among those configured for digital I/O
 *  (See io_set_analog_port_count()).
 *  On the Vex, analog ports are input only.
 *
 *  \param      port    Port to configure.
 *  \param      dir     IO_DIRECTION_IN or IO_DIRECTION_OUT.
 *
 *  \returns    OV_OK on success, OV_BAD_PARAM if port is invalid
 *              or not configured for digital I/O.
 */

/*
 * History:
 *  Dec 2008     J Bacon
 */

status_t    io_set_direction(unsigned char port,io_dir_t dir)
{
    if ( ! VALID_DIGITAL_PORT(port) )
	return OV_BAD_PARAM;
    
    switch(port)
    {
	case    1:  IO_DIRECTION1 = dir; break;
	case    2:  IO_DIRECTION2 = dir; break;
	case    3:  IO_DIRECTION3 = dir; break;
	case    4:  IO_DIRECTION4 = dir; break;
	case    5:  IO_DIRECTION5 = dir; break;
	case    6:  IO_DIRECTION6 = dir; break;
	case    7:  IO_DIRECTION7 = dir; break;
	case    8:  IO_DIRECTION8 = dir; break;
	case    9:  IO_DIRECTION9 = dir; break;
	case    10:  IO_DIRECTION10 = dir; break;
	case    11:  IO_DIRECTION11 = dir; break;
	case    12:  IO_DIRECTION12 = dir; break;
	case    13:  IO_DIRECTION13 = dir; break;
	case    14:  IO_DIRECTION14 = dir; break;
	case    15:  IO_DIRECTION15 = dir; break;
	case    16:  IO_DIRECTION16 = dir; break;
	default:
	    return OV_BAD_PARAM;
    }
    return OV_OK;
}


char    io_get_direction(unsigned char port)
{
    if ( ! VALID_DIGITAL_PORT(port) )
	return OV_BAD_PARAM;
    
    switch(port)
    {
	case    1:  return IO_DIRECTION1;
	case    2:  return IO_DIRECTION2;
	case    3:  return IO_DIRECTION3;
	case    4:  return IO_DIRECTION4;
	case    5:  return IO_DIRECTION5;
	case    6:  return IO_DIRECTION6;
	case    7:  return IO_DIRECTION7;
	case    8:  return IO_DIRECTION8;
	case    9:  return IO_DIRECTION9;
	case    10:  return IO_DIRECTION10;
	case    11:  return IO_DIRECTION11;
	case    12:  return IO_DIRECTION12;
	case    13:  return IO_DIRECTION13;
	case    14:  return IO_DIRECTION14;
	case    15:  return IO_DIRECTION15;
	case    16:  return IO_DIRECTION16;
	default:
	    return OV_BAD_PARAM;
    }
}


/****************************************************************************
 *  Set direction bits for PWMs.
 *
 * History: 
 *  Dec 2008    J Bacon     Derived from assembly language version in Vex
 *                          default code, which is not SDCC compatible.
 ***************************************************************************/

void    io_update_local_pwm_dir(unsigned char txPWM_MASK)

{
    IO_DIRECTION_PWM1 = txPWM_MASK & 0x01;
    IO_DIRECTION_PWM2 = txPWM_MASK & 0x02;
    IO_DIRECTION_PWM3 = txPWM_MASK & 0x04;
    IO_DIRECTION_PWM4 = txPWM_MASK & 0x08;
    IO_DIRECTION_PWM5 = txPWM_MASK & 0x10;
    IO_DIRECTION_PWM6 = txPWM_MASK & 0x20;
    IO_DIRECTION_PWM7 = txPWM_MASK & 0x40;
    IO_DIRECTION_PWM8 = txPWM_MASK & 0x80;
}

/** @} */

