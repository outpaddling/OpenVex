#include <spi.h>
#include <stdio.h>
#include "vex_usart.h"
#include "platform.h"
#include "io.h"
#include "vex_spi.h"
#include "master.h"
#include "timer.h"
#include "init.h"

/**
 *  \addtogroup controller
 *  @{
 */
 
/**
 *  Initialize Vex hardware.  Must be called before accessing any
 *  controller ports.
 */

/*
 * History: 
 *  Dec 2008    J Bacon     Derived from Vex default code.
 */

void    controller_init(void)
{
    unsigned char c;

    init_regs();

    /* Enable serial output to a terminal via stdio functions. */
    usart_init();

#if DEBUG_STACK
    debug_paint_stack();
#endif

    /* Make sure all motors are off when the program starts. */
    for (c=1; c<=8; ++c)
	pwm_write(c, 0);

    /* Unused pwm fields */
    for (c=8; c<16; ++c)
	User_txdata.pwm[c] = 0x7f;
	
    /* Choose which processor will control which PWM outputs. */
    for (c=1; c<=8; ++c)
	pwm_select_processor(c, PWM_MASTER);
    
    /* Turn off all timers */
    for (c=1; c<=4; ++c)
	timer_stop(c);

    User_txdata.packet_num = 0;
    User_txdata.current_mode = 1;
    User_txdata.control = 0xc9;     /* ?? From defaut code packet dump */
    Spi_status.first_time = 1;

    // Redundant? controller_submit_data(NO_WAIT);

    /*
     * IFI_PWM: Standard IFI PWM output generated with Generate_Pwms()
     *          ( for motor/servo control )
     * USER_CCP:    User can use PWM pin as digital I/O or CCP pin.
     *
     * Example: The following would generate a 40KHz PWM with a 50% duty
     * cycle on the CCP2 pin (PWM OUT 1):
     * 
     * pwm_write_USER(1);
     * CCP2CON = 0x3C; PR2 = 0xF9;
     * CCPR2L = 0x7F; T2CON = 0; T2CONbits.TMR2ON = 1;
     * Setup_PWM_Output_Type(USER_CCP,IFI_PWM,IFI_PWM,IFI_PWM)
     */
    // pwm_set_output_type(IFI_PWM, IFI_PWM, IFI_PWM, IFI_PWM);

    /* Initialize spi status struct */
    MASTER_CLR_NEW_RC_DATA_FLAG(Spi_status);

    /* Initiate communication with the master processor */
    controller_submit_data(NO_WAIT);
    user_proc_is_ready();

    while ( ! rc_new_data_available() )
	; 
    
    // Start system timer
    timer0_init();
}


/****************************************************************************
 *  What the name says.
 *  This function probably does a lot of unnecessary setup, but should
 *  not be tampered with unless you know what you're doing.
 *
 * History: 
 *  Dec 2008    J Bacon     Derived from Vex default code.
 ***************************************************************************/

void    init_regs(void)
{
    CMCON = 7;      // Disable comparator circuits.
    PORTA = PORTB = PORTC = PORTD = PORTE = PORTF = PORTG = PORTH = PORTJ = 0;
    LATA = LATB = LATC = LATD = LATE = LATF = LATG = LATH = LATJ = 0;
    TRISA = 0xff;   // PORT A tri-state all inputs
    TRISB = 0xff;   // Yada, yada...
    TRISC = 0xbf;
    TRISD = 0;
    TRISE = 0x70;
    TRISF = 0x9f;
    TRISG = 0xe4;
    TRISH = 0x0f;
    TRISJ = 0xff;
    ADCON1 = 0x0f;  // All inputs digital (ADC_0ANA)
    ADCON2 = 0x07;  // RC Fosc for A/D converter
    INTCON2bits.RBPU = 0;   // Enable PORTB pull-ups
    PSPCONbits.PSPMODE = 0; // Disable parallel slave on PORTD
    MEMCONbits.EBDIS = 1;   // Disable external memory bus
    IPR1 = IPR2 = 0;        // All peripherals use low priority interrupts
    PIE1 = PIE2 = 0;        // Disable all peripheral interrupts
    IPR3 = PIE3 = 0;        

    OpenSPI(SLV_SSOFF, MODE_01, SMPMID);

    IPR1bits.SSPIP = 1;     // Master synchronous serial port high priority
    RCONbits.IPEN = 1;      // Enable interrupt priorities
    PIR1bits.SSPIF = 0;     // Clear master sync serial flag
    PIE1bits.SSPIE = 1;     // Enable master sync serial interrupts
    TRISBbits.TRISB1 = 1;   // Set PORTB 1 as interrupt pin
    INTCON3 = 0;            
    INTCONbits.INT0E = 1;   // Enable external interrupt int0
    INTCON3bits.INT2IP = 0; // Low priority Vex port 1 interrupts
    INTCON3bits.INT2IE = 0; // Disable Vex port 1 interrupts
    INTCON2bits.INT3P = 0;  // Low priority Vex port 2 interrupts
    INTCON3bits.INT3IE = 0; // Disable Vex port 2 interrupts
    INTCON2 = 0;
    INTCONbits.PEIE = 1;    // Enable peripheral interrupts
    INTCONbits.GIE = 1;     // Enable global interrupts
}

/** @} */

