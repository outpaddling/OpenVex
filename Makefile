
# Defaults if not set by port/package
PREFIX?=    /usr/local
MANPREFIX?= ${PREFIX}
DATADIR?=   ${PREFIX}/share/openvex

all:
	cd Beginner && make depend && make
	cd Advanced && make depend && make
	cd HiBob && make depend && make
	
clean:
	cd Beginner && make clean
	cd Advanced && make clean
	cd HiBob && make clean
	rm -f .*.bak

realclean:
	cd Beginner && make realclean
	cd Advanced && make realclean
	cd HiBob && make realclean

depend:
	cd Beginner && make depend
	cd Advanced && make depend
	cd HiBob && make depend

install:
	mkdir -p ${PREFIX}/bin ${DATADIR} ${MANPREFIX}/man/man1
	cp -Rp * ${DATADIR}
	install -m 0555 openvex-new-project ${PREFIX}/bin
	install -m 0444 Doc/openvex.man ${MANPREFIX}/man/man1/openvex.1
	install -m 0444 Doc/openvex.man ${MANPREFIX}/man/man1/openvex-new-project.1

