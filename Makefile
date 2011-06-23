#  
#	Makefile -- Top level Makefile for MPR 
#
#	Copyright (c) Embedthis Software LLC, 2003-2011. All Rights Reserved.
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

diff import sync:
	$(BLD_TOOLS_DIR)/import.sh --$@ ../tools/out/releases/tools-dist.tgz

#
#   Local variables:
#   tab-width: 4
#   c-basic-offset: 4
#   End:
#   vim: sw=4 ts=4 noexpandtab
#
