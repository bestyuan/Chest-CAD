#------------------------------------------------------------------------------
#   (c) Copyright Eastman Kodak Company 
#   $Date: 2000/10/27 14:39:46 $
#   343 State Street
#   Rochester, NY 14650
#   All rights reserved
#
#   RCS: $Id: External.mk,v 1.2 2000/10/27 14:39:46 blose Exp $
#
#   Description: This is a sample makefile provided as a starting point for
#   users of the IEM library. It provides the basic building environment
#   for applications using the IEM library outside the IADG Sniff environment.
#
#   Use: To use this makefile, copy it to the directory that will contain
#   the source files for your IEM application and rename the file to 
#   "Makefile". Peruse the following section and modify it as necessary. 
#   There are a few common build targets defined. Make is commonly invoked 
#   with no other options to build the default target. To build a specific 
#   target, type
#
#   	make <target> <make options>
#
#   where <target> is one of the following:
#
#   static - This target links the application against the static libraries.
#   It should be used for builds where the executable will exist for longer
#   periods of time or where the executable may be run on other systems. For
#   safety reasons (see below), this is the default make target.
#
#   dynamic - This target links the application with  dynamic (shared object)
#   libraries. It can be used during development where fast links and small
#   executables are desired. 
#
#   WARNING: When an application is linked with a shared object
#   library, runtime dependencies are created between the exectable
#   and the library. Neither the interface nor the location of the
#   shared object library may change for the life of the executable.
#
#   This means that the executable is not easily moved to other
#   systems.  The executable may/will also 'mysteriously' stop working
#   if a new release of any of the shared object libraries it links
#   against is made.
#
#   and where <make options> can be:
#
#   USE_VIA=1 - This will link your program with the Via I/O library.
#
#   USE_IMA=1 - This will link your program with the IMA library. This must be
#   specified if IMA integration features are being used. USE_VIA and USE_IMA 
#   are mutually exclusive. If you have an extremely trivial program or wish 
#   to use your own I/O, both of these can be omitted.
#
#   JOBS=n - This will cause a make to distribute a make across a number 
#   jobs. On most single CPU systems, JOBS=2 is the fastest way to build
#   a multi-file application.
#
#   NODEBUG=1 - This will compile and or link without debug symbols.
#
#   OPTIMIZE=1 - This will turn on code optimization.
#
#   Pre-initialized versions of these appear below for convenience.
#
#   Sample Invocations:
#
#   make JOBS=2
#   make dynamic JOBS=2
#   make static NODEBUG=1 OPTIMIZE=1 JOBS=2
#
#------------------------------------------------------------------------------
#------------------------------------------------------------------------------
# You NEED to modify this section.
#
# 	PROGRAM specifies the filename of the source file containing the 
# 	main program (without the extension) and the name of the executable.
#
#	MORE_SRCS can be zero or more source files (with the extensions) 
#	that need to be compiled and linked into the program.
#
#	BINDIR is the directory in which the program will reside.
#
# ----------------------------------------------------------------------------
SRC_PATH	= 
PROGRAM		= ccICD
MAIN            = ${SRC_PATH}/ccICD.cpp 
MORE_SRCS	= ${SRC_PATH}/IcdXrayImage.cpp ${SRC_PATH}/Afum.cpp ${SRC_PATH}/TextGen.cpp ${SRC_PATH}/ImageTruth.cpp ${SRC_PATH}/TrueNodule.cpp ${SRC_PATH}/Rect.cpp ${SRC_PATH}/CanFile.cpp ${SRC_PATH}/GmlClassifier.cpp ${SRC_PATH}/Classifier.cpp ${SRC_PATH}/DMatrix.cpp ${SRC_PATH}/IcdUtils.cpp ${SRC_PATH}/IcdImgProc.cpp ${SRC_PATH}/IcdFeatureManager.cpp ${SRC_PATH}/Mhac.cpp ${SRC_PATH}/LungMask.cpp ${SRC_PATH}/CandidateList.cpp  ${SRC_PATH}/MultiscaleMhac.cpp
BINDIR		= .

# Typically, your makefile is fairly static. You can hardwire some of the make
# options here to save typing on the make command line. If you do specify these
# on the command line, they get overridden.
#
#NODEBUG		= 0
OPTIMIZE	= 1
USE_VIA 	= 1
#USE_IMA 	= 1

# ----------------------------------------------------------------------------
# You probably can (and should) leave the rest of this file as is
# ----------------------------------------------------------------------------
IADGROOT = /inhouse/iadg
PKGSROOT = ${IADGROOT}/packages
IEMROOT	 = ${PKGSROOT}/Iem/current

include ${IEMROOT}/share/iemcommon.mk

BINDIR = ./