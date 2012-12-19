#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <spi.h>
#include "platform.h"
#include "io.h"
#include "vex_spi.h"
#include "master.h"
#include "interrupts.h"

/********************************************************************
 *  Double buffers for SPI interface 
 ********************************************************************/

/*
 *  Rx_user_buff_index alone controls mutually exclusive access to the
 *  double buffers Rx_buff[2], which receive data from the master processor.
 *  The user code accesses Rx_buff[Rx_user_buff_index] while the SPI
 *  ISR code accesses Rx_buff[!Rx_user_buff_index].  This variable should
 *  only be toggled in the ISR so that the ISR will never write to the
 *  wrong buffer.
 *
 *  User_rxdata is simply a pointer to Rx_buff[Rx_user_buff_index].
 */
volatile rx_data_t      Rx_buff[2];
volatile unsigned char  Rx_user_buff_index = INITIAL_BUFF_INDEX;
volatile rx_data_t      *User_rxdata = Rx_buff + INITIAL_BUFF_INDEX;

/*
 *  Tx_user_buff_index controls mutually exclusive access to the
 *  txdata buffers.
 *
 *  Unlike the rxdata buffer mechanism, it makes sense to employ
 *  third txdata buffer, in which the user code modifies individual
 *  fields.  This buffer if mem copied to Tx_buff[Tx_user_buff_index]
 *  by tx_submit_data().  The SPI ISR transmits from the other buffer
 *  Tx_buff[!Tx_user_buff_index] to avoid conflicts.
 */
volatile tx_data_t      Tx_buff[2];
volatile unsigned char  Tx_user_buff_index = INITIAL_BUFF_INDEX;
tx_data_t               User_txdata;

volatile spi_status_t   Spi_status;

//volatile unsigned short Spi_isr_start;
//volatile unsigned short Spi_isr_end;

#ifdef SDCC
// Not necessary when compiling with --ivt-loc 0x800
#pragma code InterruptVectorHigh 0x808
#else
#pragma code InterruptVectorHigh=0x808
#endif

/****************************************************************************
 *  Interrupt service routine for high-priority interrupts.  By default,
 *  This vector is used only for SPI interrupts.  ( Communication
 *  with the master processor. )
 *
 *  The INT0 interrupt signals the start of a packet from the master.
 *  This occurs every 18.5ms.  Interrupts are triggered on the falling
 *  edge, and INT0 remains low for 17us.
 *
 *  The MSSP interrupt signals the transfer of each byte over the SPI
 *  link.  The first byte arrives 18.7us after INT0 drops, and the
 *  remaining bytes arrive every 18us, which is only 180 instruction cycles
 *  on the Vex controller.
 *
 *  This ISR takes about 11us under SDCC and 6us under MCC18.  If it is
 *  factored into separate functions, function call overhead could
 *  bring it close to the 18us limit.
 *
 * History:
 *  Dec 2008    J Bacon, Ag Primatic    Derived from Vex default code.
 *  May 2009    J Bacon     Rewrote double-buffer mechanisms
 ***************************************************************************/

void InterruptVectorHigh(void) NAKED_INTERRUPT_VECTOR(1)
{
    _ASM
    goto C_LABEL(InterruptHandlerHigh)
    _ENDASM;
}

#ifndef SDCC
#pragma code
#ifndef V240
// Must at least nosave .tmpdata and MATH_DATA
#pragma interrupt InterruptHandlerHigh nosave=TBLPTR, TABLAT, PCLATH, PROD, section(".tmpdata"), section("MATH_DATA")
#else
#pragma interrupt InterruptHandlerHigh
#endif
#endif

void    InterruptHandlerHigh(void) INTERRUPT

{
    static volatile unsigned char   *tx_ptr;
    static volatile unsigned char   *rx_ptr;
    static volatile unsigned char   Spi_byte_count;
    static unsigned char            spi_ch;
    static unsigned char            packet_num = 0;

    /*
     *  Time ISR.  TMR0 clock has a 100ns period (same as instruction cycle)
     *  Run time = Timer0 ticks * 100ns * Timer0 prescale factor
    Spi_isr_start = TMR0L;
    Spi_isr_start |= TMR0H << 8;
    
    if ( SSPCON1bits.WCOL )
	DPRINTF("SPI write collision!\n");
    if ( SSPCON1bits.SSPOV )
	DPRINTF("SPI overflow!\n");
    */
    
    /*
     *  External interrupt 0 signals start of a new SPI packet from
     *  master processor.
     */
    if (INTCONbits.INT0F)
    {
	INTCONbits.INT0F = 0;   /* Clear interrupt condition. */
	Spi_byte_count = sizeof(rx_data_t);
	IPR1bits.SSPIP = 1;     /* Set SPI for high-priority interrupts */
    
	/*
	 *  Double buffering select.  Fill the buffer that the user code
	 *  is not currently accessing.  Toggle the index and pointer
	 *  after the buffer is filled.
	 */
	rx_ptr = (volatile unsigned char *)(Rx_buff + !Rx_user_buff_index);
	
	tx_ptr = (volatile unsigned char *)(Tx_buff + !Tx_user_buff_index);
    
	// Force increment of packet_num to see if we can eliminate
	// the need for repeated tx_submit_data() to keep motors running.
	if ( User_rxdata->rc_mode.autonomous )
	{
	    Tx_buff[!Tx_user_buff_index].packet_num = packet_num;
	    Tx_buff[Tx_user_buff_index].packet_num = packet_num;
	    ++packet_num;
	}
	/* spi_ch is never used.  Just to clear BF? */
	spi_ch = SSPBUF;
	SSPBUF = *tx_ptr++;
    }
    /*
     *  SSPIF signals next byte from MSSP (SPI)
     */
    else if (PIR1bits.SSPIF)
    {
	PIR1bits.SSPIF = 0;     /* Clear SPI interrupt condition */
    
	/*
	 *  Transfer one byte in each direction over the SPI link.
	 *  Spi_byte_count starts at sizeof(rx_data_t).
	 */
	*rx_ptr++ = SSPBUF;     /* Buffer next byte from master */
	SSPBUF = *tx_ptr++;     /* Send next byte to master */
	
	/* Done receiving packet. */
	if (--Spi_byte_count == 0)
	{
	    /* 
	     * We have all the time in the world here (almost 18.5ms
	     * until the next packet is sent, so do as much in this
	     * section as possible as opposed to other parts of the
	     * ISR.
	     */
	    /* Initialized to one.  2 means this is not the first packet. */
	    User_txdata.current_mode = 2;
	    Spi_status.new_tx_data = 0;
	    MASTER_SET_NEW_RC_DATA_FLAG(Spi_status);
	    Rx_user_buff_index = !Rx_user_buff_index;
	    User_rxdata = Rx_buff + Rx_user_buff_index;
	}
	//  This appears to serve no purpose, but I'm not 100% sure.
	//  If someone put it here, they must have thought it was necessary.
	//  Uncommenting it causes a failure to enter autonomous mode.
	//  This is why people should comment their code.
	//else
	//    SSPBUF = 0;
    }
    /*
    Spi_isr_end = TMR0L;
    Spi_isr_end |= TMR0H << 8;
    */
}


#ifdef SDCC
/*
 *  In the default code, OpenSPI() gets called with:
 *  mode_select = 5 (SPI Slave mode, clock=SCK pin, SS pin control
 *               disabled),
 *  clock_mode  = 1 (set tx on active-to-idle and clock idle high), and
 *  sample      = 0 (input data sampled at middle of data output time).
 *
 *  History: 
 *      Dec 2008    Ag Primatic
 *      Dec 2008    J Bacon     Added TRISF7 = 1 per 8520 spec sheet.
 *      Dec 2008    J Bacon     Added INT0 enable out of paranoia.
 */

void    OpenSPI(unsigned char mode_select, unsigned char clock_mode, unsigned char sample)

{
    SSPCON1 = mode_select & 0x0F;       /* Also clears CKP */
    SSPSTAT = sample? 0x80: 0x00;       /* Also clears CKE */

    switch(clock_mode)
    {
	case    MODE_00:
	    SSPCON1bits.CKP = 0;
	    SSPSTATbits.CKE = 1;
	    break;
	case    MODE_01:
	    SSPCON1bits.CKP = 0;
	    SSPSTATbits.CKE = 0;
	    break;
	case    MODE_10:
	    SSPCON1bits.CKP = 1;
	    SSPSTATbits.CKE = 1;
	    break;
	case    MODE_11:
	    SSPCON1bits.CKP = 1;
	    SSPSTATbits.CKE = 0;
	    break;
    }

    /* This is lifted from the default code.  I don't know what is
     * connected to what pins on the pic.
     */
    if (mode_select == SLV_SSOFF) {
	TRISCbits.TRISC3 = 1;   /* RC3/SCK/SCL an input. 1 for SPI slave. */
	SSPSTATbits.SMP  = 0;   /* Force SMPMID */
    } else if (mode_select == SPI_FOSC_16) {
	TRISAbits.TRISA5 = 1;   /* OSC2/CLKO/RA6 an input */
    } else {
	/* mode select not 1 or 5 */
	TRISCbits.TRISC3 = 0;   /* RC3/SCK/SCL an output. 0 for SPI master. */
    }

    TRISCbits.TRISC5 = 0;       /* RC5/SDO an output. Required for SPI. */
    TRISCbits.TRISC4 = 1;       /* RC4/SDI/SDA an input */
    TRISFbits.TRISF7 = 1;       /* SS' must be set. */
    
    INTCON2bits.INTEDG0 = 0;    /* Trigger on falling edge */
    INTCONbits.INT0E = 1;       /* Enable pack interrupts */
    
    /* Enable SPI */
    SSPCON1bits.SSPEN = 1;
}
#endif


/****************************************************************************
 *  Notify master processor that user processor is initialized.  The
 *  master processor will then commence sending and receiving packets
 *  for the remote control and motor ports.
 *
 * History: 
 *  Dec 2008    J Bacon     Derived from Vex default code.
 ***************************************************************************/

void    user_proc_is_ready(void)
{
    PORTCbits.RC1 = 1;
    PORTAbits.RA4 = 1;
    TRISCbits.TRISC1 = 0;
    TRISAbits.TRISA4 = 0;
}


/****************************************************************************
 *  Validate txdata (preferably before sending data to master processor).
 *
 * History: 
 *  Dec 2008    J Bacon     Derived from Vex default code.
 ***************************************************************************/

void    check_tx_env(void)
{
    User_txdata.error_code = 0;
    User_txdata.warning_code = 0;

    /* Check for SPI write collision */
    if (SSPCON1bits.WCOL)
    {
	SSPCON1bits.WCOL = 0;
	User_txdata.warning_code = 1;
    }
    /* Check if PORTB pull-ups and RB interrupts are enabled */
    else if ((INTCON2bits.RBPU == 1) && (INTCONbits.RBIE == 1))
    {
	User_txdata.warning_code = 2;
    } 
    else if (INTCON3bits.INT1IE)
    {
	LATHbits.LATH7 = 0;         /* Digital_out16 */
	User_txdata.warning_code = 3;
    }
    
    /* SSPEN, SSPM2, SSPM0 */
    if ((SSPCON1 & 0x25) != 0x25)
    {
	User_txdata.error_code = 1;
	User_txdata.warning_code = SSPCON1;
    }
    else if (SSPCON2)
    {
	User_txdata.error_code = 2;
	User_txdata.warning_code = SSPCON2;
    }
    /* Interrupt priorities enabled? SPI uses high-priority interrupts. */
    else if (!RCONbits.IPEN)
    {
	User_txdata.error_code = 3;
	User_txdata.warning_code = RCON;
    }
    /* Master sync serial interrupts */
    else if (!PIE1bits.SSPIE)
    {
	User_txdata.error_code = 4;
	User_txdata.warning_code = PIE1;
    }
    /* External memory bus? */
    else if (MEMCON)
    {
	User_txdata.error_code = 5;
	User_txdata.warning_code = MEMCON;
    }
    /*
     *  Only master sync should use high priority interrupts.  All other
     *  INTPR bits should be 0.
     */
    else if (IPR1 != 0x08)  /* IPR1bits.SSPIP? */
    {
	User_txdata.error_code = 6;
	User_txdata.warning_code = IPR1;
    }
    else if (IPR2)
    {
	User_txdata.error_code = 7;
	User_txdata.warning_code = IPR2;
    }
    else if (IPR3)
    {
	User_txdata.error_code = 8;
	User_txdata.warning_code = IPR3;
    }
    /* int0 enabled? */
    else if ((INTCON & 0x10) != 0x10)
    {
	User_txdata.error_code = 9;
	User_txdata.warning_code = INTCON;
    }
    /* INTEDG0, T0IP, INT3P, RBIP */
    else if (INTCON2 & 0x47 )
    {
	User_txdata.error_code = 10;
	User_txdata.warning_code = INTCON2;
    }
    /* INT2P, INT1P */
    else if (INTCON3 & 0xC0 )
    {
	User_txdata.error_code = 11;
	User_txdata.warning_code = INTCON3;
    } 
    else if (TRISAbits.TRISA4)
    {
	User_txdata.error_code = 12;
	User_txdata.warning_code = TRISA;
    }
    else if (!TRISBbits.TRISB0)
    {
	User_txdata.error_code = 13;
	User_txdata.warning_code = TRISB;
    }
    /* TRISC 5, 4, 3, 2, 1 */
    else if ((TRISC & 0x3e) != 0x1c)
    {
	User_txdata.error_code = 14;
	User_txdata.warning_code = TRISC;
    }
    else if (!TRISFbits.TRISF7)
    {
	User_txdata.error_code = 15;
	User_txdata.warning_code = TRISF;
    }
}

