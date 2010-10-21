# 
#	Makefile -- Top level Makefile for MPR 
#
#	Copyright (c) Embedthis Software LLC, 2003-2010. All Rights Reserved.
#
#
#	Standard Make targets supported are:
#	
#		make 						# Does a "make compile"
#		make clean					# Removes generated objects
#		make compile				# Compiles the source
#		make depend					# Generates the make dependencies
#		make test 					# Runs unit tests
#

include		build/make/Makefile.top
include		build/make/Makefile.mpr

testFinal:
	killall testMpr >/dev/null 2>&1 ; true

diff import sync:
	if [ ! -x $(BLD_TOOLS_DIR)/edep$(BLD_BUILD_EXE) -a "$(BUILDING_CROSS)" != 1 ] ; then \
		$(MAKE) -S --no-print-directory _RECURSIVE_=1 -C $(BLD_TOP)/build/src compile ; \
	fi
	if [ "`git branch | grep master`" != "* master" ] ; then echo "Sync only in default branch" ; echo 255 ; fi
	$(BLD_TOOLS_DIR)/import.sh --$@ ../tools/releases/tools-all.tgz
