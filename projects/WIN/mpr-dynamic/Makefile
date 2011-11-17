#
#	Makefile for Windows Visual Studio projects
#
#	Copyright (c) Embedthis Software LLC, 2003-2009. All Rights Reserved.
#

include     .makedep

include     buildConfig.override

SRC			= ../../../src
CMD			= ../../../src/utils
MPR_HDR 	= $(BLD_INC_DIR)/buildConfig.h $(shell find $(SRC)/include -name 'mpr*.h')
LIBS		= ws2_32.lib
PROJECTS	= libmpr.vcproj libmprssl.vcproj testMpr.vcproj benchMpr.vcproj runProgram.vcproj
DFLAGS		= DEBUG_IDE

MAKE_IFLAGS	= $(BLD_MPR_IFLAGS) -I$(BLD_TOP)/src/include/master 
SSL_IFLAGS	+= $(BLD_OPENSSL_IFLAGS)
SSL_IFLAGS	+= $(BLD_MATRIXSSL_IFLAGS)

compileExtra: prep $(PROJECTS)

prep:
	if ls $(BLD_BIN_DIR)/*.def >/dev/null 2>&1 ; then \
		for f in $(BLD_BIN_DIR)/*.def ; do \
			cp $$f $$.tmp ; cat $$.tmp | grep -v mask@@NegDouble >$$f ; rm -f $$.tmp ; \
		done ; \
	fi

libmpr.vcproj: Makefile buildConfig.override
	@makeVsProj --project libmpr.vcproj --library mpr --dflags "$(DFLAGS)" \
		--def libmpr.def --libs "$(LIBS)" $(MPR_HDR) \
	    $(shell find $(SRC) -name '*.c' | egrep -v 'ssl/|utils|test/|manager')
	
libmprssl.vcproj: Makefile buildConfig.override
	@makeVsProj --project libmprssl.vcproj --library mprSsl --dflags "$(DFLAGS)" \
		--iflags "$(SSL_IFLAGS)" --def libmprssl.def --libs "libmpr.lib $(LIBS)" $(MPR_HDR) \
		$(SRC)/ssl/*.c
	
testMpr.vcproj: Makefile buildConfig.override
	@makeVsProj --project testMpr.vcproj --exe testMpr.exe --dflags "$(DFLAGS)" \
		--libs "$(LIBS) libmpr.lib" $(MPR_HDR) \
	    $(shell find $(SRC)/test -name '*.c' | egrep -v 'runProgram|benchMpr')

benchMpr.vcproj: Makefile buildConfig.override
	@makeVsProj --project benchMpr.vcproj --exe benchMpr.exe --dflags "$(DFLAGS)" \
		--libs "$(LIBS) libmpr.lib" $(MPR_HDR) $(SRC)/test/benchMpr.c 

runProgram.vcproj: Makefile buildConfig.override
	@makeVsProj --project runProgram.vcproj --exe runProgram.exe --dflags "$(DFLAGS)" \
		--libs "$(LIBS) libmpr.lib" $(MPR_HDR) $(SRC)/test/runProgram.c 

cleanExtra:
	rm -rf Upgrade* _Upgrade*

#
#   Local variables:
#   tab-width: 4
#   c-basic-offset: 4
#   End:
#   vim: sw=4 ts=4 noexpandtab
#
