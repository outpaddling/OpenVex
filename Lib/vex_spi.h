
#ifndef __vex_spi_h__
#define __vex_spi_h__

#define AUTONOMOUS  0
#define COMPETITION 0

#define INITIAL_BUFF_INDEX  0

#define SPI_PACKET_LEN  32

typedef struct
{
    unsigned char  :6;
    unsigned char  autonomous:1;    /* Autonomous enabled = 1, disabled = 0 */
    unsigned char  disabled:1;      /* Robot disabled = 1, enabled = 0 */
}   rc_mode_t;

/*
 * This structure defines the contents of the data received from the Master
 * microprocessor.
 */
typedef struct
{
    unsigned char packet_num;
    rc_mode_t     rc_mode;
    unsigned char rc_status_byte;
    unsigned char reserve1[3];

    unsigned char oi_analog[16];
    unsigned char reserve2[9];
    unsigned char master_version;
}   rx_data_t;

/*
 * This structure defines the contents of the data transmitted to the master
 * microprocessor.
 */
 
typedef struct

{
    unsigned char reserve1[4];
    unsigned char pwm[16];      /* PWM values, 127 = stop/center */
    unsigned char user_cmd;     /* Set autonomous mode, etc. */
    unsigned char cmd_byte1;
    unsigned char pwm_mask;
    unsigned char warning_code;
    unsigned char reserve2[4];
    unsigned char error_code;
    unsigned char packet_num;
    unsigned char current_mode; /* 1 = first packet, 2 = not */
    unsigned char control;      
}   tx_data_t;

/*
 *  Flags for SPI link between master and user processors.
 */

typedef struct
{
    unsigned char new_rc_data:1;
    unsigned char new_tx_data:1;
    unsigned char first_time:1;
    unsigned char:2;
    unsigned char semaphore:1;
    unsigned char:2;
}   spi_status_t;


extern volatile rx_data_t      Rx_buff[2];
extern volatile unsigned char  Rx_user_buff_index;
extern volatile rx_data_t      *User_rxdata;
extern volatile tx_data_t      Tx_buff[2];
extern volatile unsigned char  Tx_user_buff_index;
extern tx_data_t               User_txdata;
extern volatile spi_status_t   Spi_status;

/* spi.c */
void user_proc_is_ready(void);
unsigned char master_new_rc_data_available(void);
void check_tx_env(void);
void OpenSPI(unsigned char mode_select, unsigned char clock_mode, unsigned char sample);

#endif

