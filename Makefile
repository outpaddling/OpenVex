
# Defaults if not set by port/package
PREFIX?=    /usr/local
MANPREFIX?= ${PREFIX}
DATADIR?=   ${PREFIX}/share/openvex
MAKE?=      make

all: depend
	${MAKE} -C Beginner
	${MAKE} -C Advanced
	${MAKE} -C HiBob
	
clean:
	${MAKE} -C Beginner clean
	${MAKE} -C Advanced clean
	${MAKE} -C HiBob clean
	rm -f .*.bak

realclean:
	${MAKE} -C Beginner realclean
	${MAKE} -C Advanced realclean
	${MAKE} -C HiBob realclean

depend:
	${MAKE} -C Beginner depend
	${MAKE} -C Advanced depend
	${MAKE} -C HiBob depend

install: clean
	mkdir -p ${PREFIX}/bin ${DATADIR} ${MANPREFIX}/man/man1
	cp -Rp * ${DATADIR}
	install -m 0555 openvex-new-project ${PREFIX}/bin
	install -m 0444 Doc/openvex.man ${MANPREFIX}/man/man1/openvex.1
	install -m 0444 Doc/openvex.man ${MANPREFIX}/man/man1/openvex-new-project.1
