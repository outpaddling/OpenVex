#ifndef __general_h__
#define __general_h__

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

#define OV_OK                   0
/*
 * Must be usable by some functions that return rc positions
 * or PWM values +/- 127
 */
#define OV_BAD_PARAM            -128
#define OV_SEQUENCE_INCOMPLETE  -2

#define PWM_MIN     -127
#define PWM_MAX     +127

#define JOY_MIN     -127
#define JOY_MAX     +127

/*
 *  Port number checks.
 */

#define VALID_INTERRUPT_PORT(p) \
    ( ((p) >= 1) && ((p) <= TOTAL_INTERRUPT_PORTS) )

#define VALID_ANALOG_PORT(p) \
    ( ((p) >= 1) && ((p) <= Analog_ports) )

#define VALID_DIGITAL_PORT(p) \
    ( ((p) > Analog_ports) && ((p) <= TOTAL_IO_PORTS) )
    
#define VALID_DIGITAL_INPUT_PORT(p) \
    ( VALID_DIGITAL_PORT(p) && (io_get_direction(p) == IO_DIRECTION_IN) )

#define VALID_DIGITAL_OUTPUT_PORT(p) \
    ( VALID_DIGITAL_PORT(p) && (io_get_direction(p) == IO_DIRECTION_OUT) )

#define VALID_PWM_VAL(v) \
    ( ((v) >= PWM_MIN) && ((v) <= PWM_MAX) )

#define VALID_PWM_PORT(p) \
    ( ((p) >= 1) && ((p) <= TOTAL_PWM_PORTS) )

#define VALID_JOYSTICK_INPUT(v) \
    ( ((v) >= JOY_MIN) && ((v) <= JOY_MAX) )

#define VALID_RC_CHANNEL(c) \
    ( ((c) >= 1) && ((c) <= TOTAL_RC_CHANNELS) )

#define VALID_ENCODER_COUNT(c)  VALID_INTERRUPT_PORT(c)

typedef char    status_t;

/* Macros and manifest constants */
#include "version.h"

#endif

