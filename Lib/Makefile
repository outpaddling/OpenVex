##########################################################################
# Makefile for use with SDCC
# J Bacon
# 
# Note: You should not need to edit this makefile to accomodate your
# installation.  Simply tell it where your SDCC or MCC18 installation
# is when you run make, e.g. for SDCC installed from MacPorts:
#
#   make LOCALBASE=/opt/local
#
# To build with mcc18, use
#
#   make MCC=mcc18
#
# Take a look at Makefile.common for other settings.

include ../Include/Makefile.common

#########################################################################
# Compiler-specific definitions

include ../Include/Makefile.${MCC}_defs

########################################################################
# Build targets

all:    ${LIB}

OBJS    = init.o vex_spi.o vex_usart.o shaft_encoder.o \
	interrupts.o master.o io.o timer.o timer_simple.o sonar.o debug.o \
	vex_delay.o lvd.o arcade_drive.o accelerometer.o line_sensor.o \
	${EXTRA_LIB_OBJS}

${LIB}: ${OBJS}
	${AR} ${ARFLAGS} ${LIB} ${OBJS}

include ${MAKEFILE_DEPEND}

########################################################################
# Utility targets

include ../Include/Makefile.targets

include Makefile.${MCC}_extras

