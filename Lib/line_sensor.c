#include <stdio.h>
#include "io.h"
#include "general.h"
#include "platform.h"
#include "line_sensor.h"

extern unsigned char    Analog_ports;

/**
 * \defgroup line_sensor  Line-sensor functions
 *  @{
 *
 *  These functions provide support for the Vex line sensor package.
 *  Individual light sensors can also be read directly using io_read_analog().
 */

/**
 *  This is a convenience function to
 *  gather sensor data from up to three light sensors in one statement.
 *  It simply calls io_read_analog() for each of the specified ports
 *  and bundles the sensor data into a convenient structure variable.
 *
 *  \param  left_port   Analog port to which left line sensor is attached
 *  \param  center_port Analog port to which center line sensor is attached
 *  \param  right_port  Analog port to which right line sensor is attached
 *  \param  sensor_data Address of a line_sensor_t structure to receive
 *                      sensor data
 *
 *  If left_port, center_port, or right_port are passed a value of
 *  LINE_SENSOR_PORT_UNUSED,
 *  then no data is collected for that port, and a sensor value
 *  of zero is loaded into sensor_data for the corresponding port.
 *
 *  Example using 3 sensors:
 *
    \verbatim
    #define LINE_SENSOR_LEFT_PORT    5
    #define LINE_SENSOR_CENTER_PORT  6
    #define LINE_SENSOR_RIGHT_PORT   7
    
    line_sensor_t   line_sensor_data;
    
    line_sensor_read(LINE_SENSOR_LEFT_PORT, LINE_SENSOR_CENTER_PORT,
		    LINE_SENSOR_RIGHT_PORT, &line_sensor_data);
    \endverbatim
 *
 *  Example using 2 sensors:
 *
    \verbatim
    #define LINE_SENSOR_LEFT_PORT    5
    #define LINE_SENSOR_RIGHT_PORT   6
    
    line_sensor_t   line_sensor_data;
    
    line_sensor_read(LINE_SENSOR_LEFT_PORT, LINE_SENSOR_PORT_UNUSED,
		    LINE_SENSOR_RIGHT_PORT, &line_sensor_data);
    \endverbatim
 */

/***************************************************************************
 *  History: 
 *  Date        Name        Modification
 *  2011-06-01  Jason Bacon Begin
 ***************************************************************************/

status_t    line_sensor_read(unsigned char left_port,
		    unsigned char center_port,
		    unsigned char right_port,
		    line_sensor_t *sensor_data)

{
    if ( ! VALID_ANALOG_PORT(left_port) ||
	 ! VALID_ANALOG_PORT(center_port) ||
	 ! VALID_ANALOG_PORT(right_port) )
	return OV_BAD_PARAM;
    
    sensor_data->left = left_port == 0 ? 0 : io_read_analog(left_port);
    sensor_data->center = center_port == 0 ? 0 : io_read_analog(center_port);
    sensor_data->right = right_port == 0 ? 0 : io_read_analog(right_port);
    return OV_OK;
}

/** @} */

