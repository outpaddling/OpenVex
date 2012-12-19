##########################################################################
# Makefile for use with SDCC and MCC18
# J Bacon
# 
# Note: You should not need to edit this makefile to accomodate your
# installation.  Simply tell it where your SDCC or MCC18 installation
# is when you run make, e.g. for SDCC installed from MacPorts:
#
#   make LOCALBASE=/opt/local
#
# The default LOCALBASE is /usr/local.
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

BINSTEM = firmware
BIN     = ${BINSTEM}.hex
LIBDIR  = ../Lib

${BIN}: ${FIRMWARE_OBJS} ${LIBDIR}/${LIB}
	${LD_CMD}

${LIBDIR}/${LIB}:
	(cd ${LIBDIR}; make LOCALBASE=${LOCALBASE} LOCALBASE=${LOCALBASE} DEBUG=${DEBUG} MCC=${MCC})

include ${MAKEFILE_DEPEND}

# Cygwin users: The serial port generated for the Prolific adapter
# will depend on which USB port you plug into.  Typically it will be
# /dev/ttyS3, /dev/ttyS4, or /dev/ttyS5.  Pick a favorite port,
# edit the line below to match, and use it with the install target.
#
#       vexctl --monitor --dev /dev/ttyS3 ${BIN}

install: ${BIN}
	vexctl --monitor upload ${BIN}

########################################################################
# Housekeeping targets such as clean, depend, etc.

include ../Include/Makefile.targets

