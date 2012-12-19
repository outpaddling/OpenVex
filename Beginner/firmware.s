	.file	"firmware.c"
	.section	.debug_abbrev,"",@progbits
.Ldebug_abbrev0:
	.section	.debug_info,"",@progbits
.Ldebug_info0:
	.section	.debug_line,"",@progbits
.Ldebug_line0:
	.text
.Ltext0:
	.p2align 4,,15
.globl main
	.type	main, @function
main:
.LFB2:
	.file 1 "firmware.c"
	.loc 1 59 0
	leal	4(%esp), %ecx
.LCFI0:
	andl	$-16, %esp
	pushl	-4(%ecx)
.LCFI1:
	pushl	%ebp
.LCFI2:
	movl	%esp, %ebp
.LCFI3:
	pushl	%ecx
.LCFI4:
	subl	$4, %esp
.LCFI5:
	.loc 1 66 0
	call	vex_init
	.loc 1 69 0
	call	custom_init
.L2:
	.loc 1 93 0
	call	rc_new_data_available
	testl	%eax, %eax
	je	.L2
	.loc 1 95 0
	call	rc_routine
	.loc 1 98 0
	movl	$5, (%esp)
	call	rc_get_data
	cmpl	$-127, %eax
	jne	.L2
	.loc 1 99 0
	call	autonomous_routine0
	.loc 1 101 0
	jmp	.L2
.LFE2:
	.size	main, .-main
	.section	.rodata
	.align 4
.LC0:
	.string	"ET: %ld  RC: %d %d %d %d %d %d %d  Jumper: %d\n"
	.align 4
.LC1:
	.string	"Starting autonomous routine...\n"
.LC2:
	.string	"Ending autonomous routine...\n"
