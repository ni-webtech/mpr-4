#
#	Makefile for the Multithreaded Portable Runtime (MPR) library
#
#	Copyright (c) Embedthis Software LLC, 2003-2011. All Rights Reserved.
#

include			.makedep

PRE_DIRS		+= deps
ifeq ($(BLD_FEATURE_SSL),1)
	PRE_DIRS	+= ssl
endif
POST_DIRS		+= utils test

TARGETS			+= $(BLD_LIB_DIR)/libmpr$(BLD_LIB)
ifeq ($(BLD_FEATURE_SSL),1)
	TARGETS		+= $(BLD_LIB_DIR)/libmprssl$(BLD_LIB)
endif
TARGETS			+= $(BLD_BIN_DIR)/angel$(BLD_EXE)

SSL_SOURCES		+= ssl/mprSsl.c
ifeq ($(BLD_FEATURE_MATRIXSSL),1)
	SSL_SOURCES	+= ssl/mprMatrixssl.c
endif
ifeq ($(BLD_FEATURE_OPENSSL),1)
	SSL_SOURCES	+= ssl/mprOpenssl.c
endif
MPR_OBJECTS		+= $(patsubst %.c,$(BLD_OBJ_DIR)/%$(BLD_OBJ),$(shell ls -1 *.c | egrep -v 'angel'))
SSL_OBJECTS		+= $(patsubst %.c,$(BLD_OBJ_DIR)/%$(BLD_OBJ),$(notdir $(SSL_SOURCES)))
DEP_OBJECTS		+= $(BLD_OBJ_DIR)/dtoa$(BLD_OBJ)
SYSLIBS			+= shell32.lib

compileFirst: ttt
	$(call log) [Notice] "Building For Target"

#
#	Build the targets in this directory first then the builtin packages (ssl and regexp)
#
compileExtra: $(TARGETS) 

ttt:
	if [ `hostname` = "phobos" ] ; then \
		echo WARN TO STDERR >&2; \
		echo HELLO TO STDOUT; \
		echo SLEEPING; \
		sleep 3600 ; \
	fi ; true

#
#	Build the mpr library
#
$(BLD_LIB_DIR)/libmpr$(BLD_LIB): $(MPR_OBJECTS)
	bld --library $(BLD_LIB_DIR)/libmpr --search "$(BLD_MPR_LIBPATHS)" --libs "$(BLD_MPR_WITHLIBS)" \
		$(MPR_OBJECTS) $(DEP_OBJECTS)

$(BLD_LIB_DIR)/libmprssl$(BLD_LIB): $(SSL_OBJECTS)
	bld --library $(BLD_LIB_DIR)/libmprssl --search "$(BLD_SSL_WITHPATHS)" --libs "$(BLD_MPR_LIBS) $(BLD_SSL_WITHLIBS)" \
		$(SSL_OBJECTS)

$(BLD_BIN_DIR)/angel$(BLD_EXE): $(BLD_LIB_DIR)/libmpr$(BLD_LIB) $(BLD_OBJ_DIR)/angel$(BLD_OBJ)
ifeq ($(BLD_UNIX_LIKE),1)
	bld --exe $(BLD_BIN_DIR)/angel$(BLD_EXE) --libs "$(BLD_MPR_LIBS)" angel$(BLD_OBJ)
endif
ifeq ($(BLD_WIN_LIKE),1)
	bld --graphical --exe $(BLD_BIN_DIR)/angel$(BLD_EXE) --rpath "$(BLD_LIB_PREFIX)" --rpath "../bin" \
		--search "$(LIBPATH)" $(MODE) --libs "mpr" --syslibs "$(SYSLIBS)" angel$(BLD_OBJ) 
endif

#
#   Local variables:
#   tab-width: 4
#   c-basic-offset: 4
#   End:
#   vim: sw=4 ts=4 noexpandtab
#

