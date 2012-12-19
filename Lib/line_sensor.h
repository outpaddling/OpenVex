
#ifndef __line_sensor_h__
#define __line_sensor_h__

/**
 * \addtogroup line_sensor
 *  @{
 */

/**
 *  Sentinal value for port number passed to left_port, center_port,
 *  right_port
 */

#define LINE_SENSOR_PORT_UNUSED 0

typedef struct
{
    unsigned short  left;
    unsigned short  center;
    unsigned short  right;
}   line_sensor_t;

/** @} */

status_t    line_sensor_read(unsigned char left_port,
		    unsigned char center_port,
		    unsigned char right_port,
		    line_sensor_t *sensor_data);

#endif

