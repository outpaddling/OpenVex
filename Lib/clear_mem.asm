#include        p18f8520.inc

	radix   dec

; configuration registers
	__config        _CONFIG1H, 0xfe
	__config        _CONFIG2L, 0xfe
	__config        _CONFIG2H, 0xfc
	__config        _CONFIG3H, 0xfe
	__config        _CONFIG4L, 0xfb
	__config        _CONFIG5L, 0xff
	__config        _CONFIG5H, 0xff
	__config        _CONFIG6L, 0xff
	__config        _CONFIG6H, 0xff
	__config        _CONFIG7L, 0xff
	__config        _CONFIG7H, 0xff

COPY_VAR        udata   0x80

CLEAR_MEM       code

	global  Clear_Memory

;*****************************************************************************
;*****************************************************************************
Clear_Memory
	movlw   0
	movwf   FSR0L
	movwf   FSR0H

Clear_Loop
	clrwdt
	call    Clear_Bank
	incf    FSR0H, f
	movlw   0x8
	subwf   FSR0H, w
	btfss   STATUS, Z
	goto    Clear_Loop
	return

Clear_Bank
	clrf    INDF0
	incfsz  FSR0L, f
	goto    Clear_Bank
	return

	end

