;--------------------------------------------------------
; File Created by SDCC : free open source ANSI-C Compiler
; Version 2.8.0 #5117 (Jan  7 2009) (Mac OS X i386)
; This file was generated Mon May 18 08:15:42 2009
;--------------------------------------------------------
; PIC16 port for the Microchip 16-bit core micros
;--------------------------------------------------------
	list	p=18f8520

	radix dec

;--------------------------------------------------------
; public variables in this module
;--------------------------------------------------------
	global _vex_init
	global _init_regs

;--------------------------------------------------------
; extern variables in this module
;--------------------------------------------------------
	extern _stdin
	extern _stdout
	extern _RCSTA2bits
	extern _TXSTA2bits
	extern _CCP5CONbits
	extern _CCP4CONbits
	extern _T4CONbits
	extern _PORTAbits
	extern _PORTBbits
	extern _PORTCbits
	extern _PORTDbits
	extern _PORTEbits
	extern _PORTFbits
	extern _PORTGbits
	extern _PORTHbits
	extern _PORTJbits
	extern _LATAbits
	extern _LATBbits
	extern _LATCbits
	extern _LATDbits
	extern _LATEbits
	extern _LATFbits
	extern _LATGbits
	extern _LATHbits
	extern _LATJbits
	extern _TRISAbits
	extern _TRISBbits
	extern _TRISCbits
	extern _TRISDbits
	extern _TRISEbits
	extern _TRISFbits
	extern _TRISGbits
	extern _TRISHbits
	extern _TRISJbits
	extern _MEMCONbits
	extern _PIE1bits
	extern _PIR1bits
	extern _IPR1bits
	extern _PIE2bits
	extern _PIR2bits
	extern _IPR2bits
	extern _PIE3bits
	extern _PIR3bits
	extern _IPR3bits
	extern _EECON1bits
	extern _RCSTA1bits
	extern _TXSTA1bits
	extern _RCSTAbits
	extern _TXSTAbits
	extern _PSPCONbits
	extern _T3CONbits
	extern _CMCONbits
	extern _CVRCONbits
	extern _CCP3CONbits
	extern _CCP2CONbits
	extern _CCP1CONbits
	extern _ADCON2bits
	extern _ADCON1bits
	extern _ADCON0bits
	extern _SSPCON2bits
	extern _SSPCON1bits
	extern _SSPSTATbits
	extern _T2CONbits
	extern _T1CONbits
	extern _RCONbits
	extern _WDTCONbits
	extern _LVDCONbits
	extern _OSCCONbits
	extern _STATUSbits
	extern _INTCON3bits
	extern _INTCON2bits
	extern _INTCONbits
	extern _STKPTRbits
	extern _Spi_status
	extern _User_txdata
	extern _User_rxdata
	extern _RCSTA2
	extern _TXSTA2
	extern _TXREG2
	extern _RCREG2
	extern _SPBRG2
	extern _CCP5CON
	extern _CCPR5L
	extern _CCPR5H
	extern _CCP4CON
	extern _CCPR4L
	extern _CCPR4H
	extern _T4CON
	extern _PR4
	extern _TMR4
	extern _PORTA
	extern _PORTB
	extern _PORTC
	extern _PORTD
	extern _PORTE
	extern _PORTF
	extern _PORTG
	extern _PORTH
	extern _PORTJ
	extern _LATA
	extern _LATB
	extern _LATC
	extern _LATD
	extern _LATE
	extern _LATF
	extern _LATG
	extern _LATH
	extern _LATJ
	extern _TRISA
	extern _TRISB
	extern _TRISC
	extern _TRISD
	extern _TRISE
	extern _TRISF
	extern _TRISG
	extern _TRISH
	extern _TRISJ
	extern _MEMCON
	extern _PIE1
	extern _PIR1
	extern _IPR1
	extern _PIE2
	extern _PIR2
	extern _IPR2
	extern _PIE3
	extern _PIR3
	extern _IPR3
	extern _EECON1
	extern _EECON2
	extern _EEDATA
	extern _EEADR
	extern _EEADRH
	extern _RCSTA1
	extern _TXSTA1
	extern _TXREG1
	extern _RCREG1
	extern _SPBRG1
	extern _RCSTA
	extern _TXSTA
	extern _TXREG
	extern _RCREG
	extern _SPBRG
	extern _PSPCON
	extern _T3CON
	extern _TMR3L
	extern _TMR3H
	extern _CMCON
	extern _CVRCON
	extern _CCP3CON
	extern _CCPR3L
	extern _CCPR3H
	extern _CCP2CON
	extern _CCPR2L
	extern _CCPR2H
	extern _CCP1CON
	extern _CCPR1L
	extern _CCPR1H
	extern _ADCON2
	extern _ADCON1
	extern _ADCON0
	extern _ADRESL
	extern _ADRESH
	extern _SSPCON2
	extern _SSPCON1
	extern _SSPSTAT
	extern _SSPADD
	extern _SSPBUF
	extern _T2CON
	extern _PR2
	extern _TMR2
	extern _T1CON
	extern _TMR1L
	extern _TMR1H
	extern _RCON
	extern _WDTCON
	extern _LVDCON
	extern _OSCCON
	extern _T0CON
	extern _TMR0L
	extern _TMR0H
	extern _STATUS
	extern _FSR2L
	extern _FSR2H
	extern _PLUSW2
	extern _PREINC2
	extern _POSTDEC2
	extern _POSTINC2
	extern _INDF2
	extern _BSR
	extern _FSR1L
	extern _FSR1H
	extern _PLUSW1
	extern _PREINC1
	extern _POSTDEC1
	extern _POSTINC1
	extern _INDF1
	extern _WREG
	extern _FSR0L
	extern _FSR0H
	extern _PLUSW0
	extern _PREINC0
	extern _POSTDEC0
	extern _POSTINC0
	extern _INDF0
	extern _INTCON3
	extern _INTCON2
	extern _INTCON
	extern _PRODL
	extern _PRODH
	extern _TABLAT
	extern _TBLPTRL
	extern _TBLPTRH
	extern _TBLPTRU
	extern _PCL
	extern _PCLATH
	extern _PCLATU
	extern _STKPTR
	extern _TOSL
	extern _TOSH
	extern _TOSU
	extern _usart_init
	extern _user_proc_is_ready
	extern _OpenSPI
	extern _rc_new_data_available
	extern _pwm_set
	extern _pwm_select_processor
	extern _pwm_set_output_type
	extern _master_submit_data
;--------------------------------------------------------
;	Equates to used internal registers
;--------------------------------------------------------
STATUS	equ	0xfd8
FSR1L	equ	0xfe1
FSR2L	equ	0xfd9
POSTDEC1	equ	0xfe5
PREINC1	equ	0xfe4


; Internal registers
.registers	udata_ovr	0x0000
r0x00	res	1

;--------------------------------------------------------
; global & static initialisations
;--------------------------------------------------------
; I code from now on!
; ; Starting pCode block
S_ifi_init__init_regs	code
_init_regs:
;	.line	110; ifi_init.c	void    init_regs(void)
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
;	.line	112; ifi_init.c	CMCON = 7;      // Disable comparator circuits.
	MOVLW	0x07
	MOVWF	_CMCON
;	.line	113; ifi_init.c	PORTA = PORTB = PORTC = PORTD = PORTE = PORTF = PORTG = PORTH = PORTJ = 0;
	CLRF	_PORTJ
	CLRF	_PORTH
	CLRF	_PORTG
	CLRF	_PORTF
	CLRF	_PORTE
	CLRF	_PORTD
	CLRF	_PORTC
	CLRF	_PORTB
	CLRF	_PORTA
;	.line	114; ifi_init.c	LATA = LATB = LATC = LATD = LATE = LATF = LATG = LATH = LATJ = 0;
	CLRF	_LATJ
	CLRF	_LATH
	CLRF	_LATG
	CLRF	_LATF
	CLRF	_LATE
	CLRF	_LATD
	CLRF	_LATC
	CLRF	_LATB
	CLRF	_LATA
;	.line	115; ifi_init.c	TRISA = 0xff;   // PORTA all inputs
	MOVLW	0xff
	MOVWF	_TRISA
;	.line	116; ifi_init.c	TRISB = 0xff;   // Yada, yada...
	MOVLW	0xff
	MOVWF	_TRISB
;	.line	117; ifi_init.c	TRISC = 0xbf;
	MOVLW	0xbf
	MOVWF	_TRISC
;	.line	118; ifi_init.c	TRISD = 0;
	CLRF	_TRISD
;	.line	119; ifi_init.c	TRISE = 0x70;
	MOVLW	0x70
	MOVWF	_TRISE
;	.line	120; ifi_init.c	TRISF = 0x9f;
	MOVLW	0x9f
	MOVWF	_TRISF
;	.line	121; ifi_init.c	TRISG = 0xe4;
	MOVLW	0xe4
	MOVWF	_TRISG
;	.line	122; ifi_init.c	TRISH = 0x0f;
	MOVLW	0x0f
	MOVWF	_TRISH
;	.line	123; ifi_init.c	TRISJ = 0xff;
	MOVLW	0xff
	MOVWF	_TRISJ
;	.line	124; ifi_init.c	ADCON1 = 0x0f;  // All inputs digital (ADC_0ANA)
	MOVLW	0x0f
	MOVWF	_ADCON1
;	.line	125; ifi_init.c	ADCON2 = 0x07;  // RC Fosc for A/D converter
	MOVLW	0x07
	MOVWF	_ADCON2
;	.line	126; ifi_init.c	INTCON2bits.RBPU = 0;   // Enable PORTB pull-ups
	BCF	_INTCON2bits, 7
;	.line	127; ifi_init.c	PSPCONbits.PSPMODE = 0; // Disable parallel slave on PORTD
	BCF	_PSPCONbits, 4
;	.line	128; ifi_init.c	MEMCONbits.EBDIS = 1;   // Disable external memory bus
	BSF	_MEMCONbits, 7
;	.line	129; ifi_init.c	IPR1 = IPR2 = 0;        // All peripherals use low priority interrupts
	CLRF	_IPR2
	CLRF	_IPR1
;	.line	130; ifi_init.c	PIE1 = PIE2 = 0;        // Disable all peripheral interrupts
	CLRF	_PIE2
	CLRF	_PIE1
;	.line	131; ifi_init.c	IPR3 = PIE3 = 0;        
	CLRF	_PIE3
	CLRF	_IPR3
;	.line	133; ifi_init.c	OpenSPI(SLV_SSOFF, MODE_01, SMPMID);
	MOVLW	0x00
	MOVWF	POSTDEC1
	MOVLW	0x01
	MOVWF	POSTDEC1
	MOVLW	0x05
	MOVWF	POSTDEC1
	CALL	_OpenSPI
	MOVLW	0x03
	ADDWF	FSR1L, F
;	.line	135; ifi_init.c	IPR1bits.SSPIP = 1;     // Master synchronous serial port high priority
	BSF	_IPR1bits, 3
;	.line	136; ifi_init.c	RCONbits.IPEN = 1;      // Enable interrupt priorities
	BSF	_RCONbits, 7
;	.line	137; ifi_init.c	PIR1bits.SSPIF = 0;     // Clear master sync serial flag
	BCF	_PIR1bits, 3
;	.line	138; ifi_init.c	PIE1bits.SSPIE = 1;     // Enable master sync serial interrupts
	BSF	_PIE1bits, 3
;	.line	139; ifi_init.c	TRISBbits.TRISB1 = 1;   // Set PORTB 1 as interrupt pin
	BSF	_TRISBbits, 1
;	.line	140; ifi_init.c	INTCON3 = 0;            
	CLRF	_INTCON3
;	.line	141; ifi_init.c	INTCONbits.INT0E = 1;   // Enable external interrupt int0
	BSF	_INTCONbits, 4
;	.line	142; ifi_init.c	INTCON3bits.INT2IP = 0; // Low priority Vex port 1 interrupts
	BCF	_INTCON3bits, 7
;	.line	143; ifi_init.c	INTCON3bits.INT2IE = 0; // Disable Vex port 1 interrupts
	BCF	_INTCON3bits, 4
;	.line	144; ifi_init.c	INTCON2bits.INT3P = 0;  // Low priority Vex port 2 interrupts
	BCF	_INTCON2bits, 1
;	.line	145; ifi_init.c	INTCON3bits.INT3IE = 0; // Disable Vex port 2 interrupts
	BCF	_INTCON3bits, 5
;	.line	146; ifi_init.c	INTCON2 = 0;
	CLRF	_INTCON2
;	.line	147; ifi_init.c	INTCONbits.PEIE = 1;    // Enable peripheral interrupts
	BSF	_INTCONbits, 6
;	.line	148; ifi_init.c	INTCONbits.GIE = 1;     // Enable global interrupts
	BSF	_INTCONbits, 7
	MOVFF	PREINC1, FSR2L
	RETURN	

; ; Starting pCode block
S_ifi_init__vex_init	code
_vex_init:
;	.line	28; ifi_init.c	void    vex_init(void)
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
	MOVFF	r0x00, POSTDEC1
;	.line	32; ifi_init.c	init_regs();
	CALL	_init_regs
;	.line	35; ifi_init.c	usart_init();
	CALL	_usart_init
;	.line	42; ifi_init.c	for (c=1; c<=8; ++c)
	MOVLW	0x01
	MOVWF	r0x00
_00108_DS_:
	MOVLW	0x09
	SUBWF	r0x00, W
	BTFSC	STATUS, 0
	GOTO	_00111_DS_
;	.line	43; ifi_init.c	pwm_set(c, 0);
	MOVLW	0x00
	MOVWF	POSTDEC1
	MOVF	r0x00, W
	MOVWF	POSTDEC1
	CALL	_pwm_set
	MOVLW	0x02
	ADDWF	FSR1L, F
;	.line	42; ifi_init.c	for (c=1; c<=8; ++c)
	INCF	r0x00, F
	GOTO	_00108_DS_
_00111_DS_:
;	.line	46; ifi_init.c	for (c=1; c<=8; ++c)
	MOVLW	0x01
	MOVWF	r0x00
_00112_DS_:
	MOVLW	0x09
	SUBWF	r0x00, W
	BTFSC	STATUS, 0
	GOTO	_00115_DS_
;	.line	47; ifi_init.c	pwm_select_processor(c, PWM_MASTER); 
	MOVLW	0x01
	MOVWF	POSTDEC1
	MOVF	r0x00, W
	MOVWF	POSTDEC1
	CALL	_pwm_select_processor
	MOVLW	0x02
	ADDWF	FSR1L, F
;	.line	46; ifi_init.c	for (c=1; c<=8; ++c)
	INCF	r0x00, F
	GOTO	_00112_DS_
_00115_DS_:
	BANKSEL	(_User_txdata + 29)
;	.line	49; ifi_init.c	User_txdata.packet_num = 0;
	CLRF	(_User_txdata + 29), B
;	.line	50; ifi_init.c	User_txdata.current_mode = 1;
	MOVLW	0x01
	BANKSEL	(_User_txdata + 30)
	MOVWF	(_User_txdata + 30), B
;	.line	51; ifi_init.c	User_txdata.control = 0xc9;
	MOVLW	0xc9
	BANKSEL	(_User_txdata + 31)
	MOVWF	(_User_txdata + 31), B
	BANKSEL	_Spi_status
;	.line	52; ifi_init.c	Spi_status.first_time = 1;
	BSF	_Spi_status, 2, B
;	.line	68; ifi_init.c	master_submit_data();
	CALL	_master_submit_data
	BANKSEL	_Spi_status
;	.line	69; ifi_init.c	Spi_status.new_tx_data = 0;
	BCF	_Spi_status, 1, B
;	.line	70; ifi_init.c	master_submit_data();
	CALL	_master_submit_data
	BANKSEL	_Spi_status
;	.line	71; ifi_init.c	Spi_status.new_tx_data = 0;
	BCF	_Spi_status, 1, B
;	.line	86; ifi_init.c	pwm_set_output_type(IFI_PWM, IFI_PWM, IFI_PWM, IFI_PWM);
	MOVLW	0x00
	MOVWF	POSTDEC1
	MOVLW	0x00
	MOVWF	POSTDEC1
	MOVLW	0x00
	MOVWF	POSTDEC1
	MOVLW	0x00
	MOVWF	POSTDEC1
	MOVLW	0x00
	MOVWF	POSTDEC1
	MOVLW	0x00
	MOVWF	POSTDEC1
	MOVLW	0x00
	MOVWF	POSTDEC1
	MOVLW	0x00
	MOVWF	POSTDEC1
	CALL	_pwm_set_output_type
	MOVLW	0x08
	ADDWF	FSR1L, F
	BANKSEL	_Spi_status
;	.line	89; ifi_init.c	MASTER_CLR_NEW_RC_DATA_FLAG(Spi_status);
	BCF	_Spi_status, 0, B
;	.line	92; ifi_init.c	master_submit_data();
	CALL	_master_submit_data
;	.line	93; ifi_init.c	user_proc_is_ready();
	CALL	_user_proc_is_ready
_00105_DS_:
;	.line	96; ifi_init.c	while ( ! rc_new_data_available() )
	CALL	_rc_new_data_available
	MOVWF	r0x00
	MOVF	r0x00, W
	BTFSC	STATUS, 2
	GOTO	_00105_DS_
	MOVFF	PREINC1, r0x00
	MOVFF	PREINC1, FSR2L
	RETURN	



; Statistics:
; code size:	  366 (0x016e) bytes ( 0.28%)
;           	  183 (0x00b7) words
; udata size:	    0 (0x0000) bytes ( 0.00%)
; access size:	    1 (0x0001) bytes


	end
