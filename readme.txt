

This goal of this project is to develop sample firmware code for VEX robots
which is easier for students and mentors to learn and modify, and easier
for developers to enhance and port to different compilers and applications.

It is not my intention to take all the fun out of robotics programming,
so I have no plans to develop this library to a very high level.

The goal of the library is to provide easy access to the Vex hardware
through a simple, intuitive, and descriptive API.  It also provides some
clean, simple examples of device drivers and ISRs.

In order to achieve these goals, the code is designed with the following
properties in mind:

1.  A clean, consistent API which is independent from controller design
    and compiler details.

2.  Adherence to established software engineering practices.

3.  Utilization of object-oriented design principles, such as
    data-hiding, abstraction, etc.

4.  Descriptive, self-documenting identifier names.

The code is confirmed to work the SDCC 2.7.0 or later and MCC18 3 or later.
Parts are derived from Sutekh's open-source library (vex_libs-0.1) and
the Vex default code, but most of the code has been completely rewritten.

SDCC vs. MCC18

SDCC has the following advantages:

1.  Free and open source.
2.  Runs on any platform (MCC18 is only for Windows).
3.  No silly 62-character pathname limitations.
4.  Compiler returns success/failure status, so rational build/install
    systems can be created.

The only advantage I've found to MCC18 is that it emits much tighter
machine code.  The SDCC PIC16 port is very new, and the code generator
is still pretty simple.  The sample_code.hex produced by SDCC is about 19k
while the MCC18 output is about 11k.  

This is probably not an issue for most programs, since most of the code
size is in the OpenVex library. The Vex has 30k of total program memory,
and it would be quite a feat to fill the remaining 11k with OpenVex API
function calls and robot logic.  It's also possible that by the time you
read this, the SDCC code generator and optimizer will be greatly improved.


============
Installation
============
To use OpenVex, you will need the following:

1.  The Vex programming hardware (cables, dongle) which comes with the
    commercial programming kits, or sold separately (P/N: 276-2186).
    
2.  SDCC (Small Device C Compiler) or MCC18

3.  A progamming tool
    Windows:
	IFI Loader
    Unix/Mac/Windows (Cygwin):
	vexctl (part of the roboctl project) 

4. A terminal emulator
    Windows: IFI Loader include a terminal app, or you can use PuTTY.
    Unix/Mac: I recommend cutecom, but you can also use PuTTY, or
	      any other terminal emulator with serial port support.

For best results, FreeBSD and Mac users should install SDCC and roboctl
via FreeBSD ports (http://freebsd.org/ports) and MacPorts
(http://macports.org).

The robotize_* scripts will assist you in setting up various systems.

Other *nix users may find SDCC and roboctl in their operating system's
package collection.

If not, roboctl can be installed manually after downloading from
http://personalpages.tds.net/~jwbacon/Ports/distfiles/.

SDCC can be installed manually via the instructions at
http://sdcc.sourceforge.net/.


==================================
Compiling with SDCC on Unix or Mac
==================================

You will need:

1. Any system that runs SDCC (this is pretty much any computer that works.)
2. SDCC

Building
========

Just build:             make
Build and upload:       make install
Clean:                  make clean


===========================================
Compiling with MCC18 on Unix or Mac systems
===========================================

You will need:

1. An operating system capable of running Wine. (This generally means a
   Unix-like operating system on an Intel or AMD processor.)
2. Wine
3. The mcc18_wrapper scripts installed in your PATH.

On FreeBSD or Mac, install the lang/mcc18_wrapper port from jb_ports.
(See http://personalpages.tds.net/~jwbacon/Ports/)

For other Unix systems, the distfile for mcc18_wrapper can be obtained
from http://personalpages.tds.net/~jwbacon/Ports/distfiles, and you
can install the wrapper scripts manually.  ( e.g. into ~/bin )


Building with MCC18
===================
First, move this directory to a location with a relatively short path.
The MCC18 compiler has a 62-character limit on pathnames.  Even if you
use short, relative pathnames in the Makefile, MCC will convert them
to full pathnames, and you can easily run into trouble.  On my FreeBSD
and Mac systems, I had to place this directory directly under my home
directory AND keep the directory name itself fairly short.

Just build:             make -f Makefile.mcc18
Build and upload:       make -f Makefile.mcc18 install
Clean:                  make clean


=============================
Compiling with MCC18 in MPLAB
=============================

MPLAB 8.50 and MCC18 3.35 Lite can be used under Windows and under
*nix with Wine.  Sample project files are provided.  Note that MPLAB
uses absolute paths for many things, so if you move the OpenVex folder
you may need to edit the project settings.  The sample project files
assume were built inside z:\OpenVex.


=========================
Starting your own project
=========================

To start your own project:

1. Make a copy of the Sample directory:

cp -R Sample New_project

2. Rename sample_code.c and sample_code.h
3. Edit the Makefile to match


===========
Programming
===========

You can use the vexctl command (part of the roboctl suite) to upload
the .hex file to your robot.  The roboctl suite is available as a FreeBSD
port and a MacPort, and is also tested on Xubuntu and Gentoo Linux.


=========
Exercises
=========

The file exercises.odt contains a series of simple programming exercises
to help you learn the basics of motor control and sensor input.  You will
need an open standards compliant word processor to read the file.  There
are several FREE programs available for this, such as OpenOffice, Abiword,
KDE Office, and GnomeOffice.

