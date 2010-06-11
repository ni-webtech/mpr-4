#
#	Makefile for the all-in-one MPR distribution
#
#	Copyright (c) Embedthis Software LLC, 2003-2010. All Rights Reserved.
#

include			.makedep

COMPILE			=
STAGING			:= staging
DEST			:= $(STAGING)/src/deps/mpr
INC_DEST		:= $(STAGING)/src/include
DOC_DEST		:= $(STAGING)/doc
TARGETS			+= $(patsubst %,$(INC_DEST)/%, mpr.h mprSsl.h)
TARGETS			+= $(patsubst %,$(DEST)/%, mprLib.c makerom.c mprSsl.c)
ARCHIVE			+= $(BLD_TOP)/releases/mpr-all.tgz

#
#	Order of headers matters
#
HEADERS			+= $(patsubst %,../src/include/%, mprOs.h mprTune.h mpr.h mprTest.h)
MPR_SOURCES 	+= $(wildcard ../src/*.c ../src/deps/*.c) 
SSL_SOURCES 	+= ../src/include/mprSsl.h $(wildcard ../src/ssl/*.c) 

packageExtra: 	prep $(TARGETS) $(ARCHIVE)

prep:
	rm -fr $(STAGING)
	mkdir -p $(DEST) $(INC_DEST) $(DOC_DEST)/man $(DOC_DEST)/api

$(INC_DEST)/mpr.h: Makefile $(HEADERS)
	all-in-one $(HEADERS) | egrep -v '#include.*mpr|#inc.*config.h|#inc.*ucp.*\.h|#inc.*pcre' >$(INC_DEST)/mpr.h

$(INC_DEST)/mprSsl.h: Makefile $(HEADERS)
	cp ../src/include/mprSsl.h $(INC_DEST)/mprSsl.h

$(DEST)/mprLib.c: Makefile $(HEADERS) $(MPR_SOURCES)
	echo '#include "mpr.h"' >$(DEST)/mprLib.c
	all-in-one $(MPR_SOURCES) | egrep -v '#inc.*mpr' >>$(DEST)/mprLib.c

$(DEST)/makerom.c: Makefile ../src/utils/makerom.c
	cp ../src/utils/makerom.c $(DEST)/makerom.c

$(DEST)/mprSsl.c: Makefile ../src/include/mprSsl.h $(HEADERS) $(SSL_SOURCES)
	all-in-one $(HEADERS) $(SSL_SOURCES) | egrep -v '#inc.*mpr' >$(DEST)/mprSsl.c

$(ARCHIVE): Makefile $(TARGETS)
	cp $(BLD_TOP)/doc/api/mprBare.html $(STAGING)/doc/api
	cp $(BLD_TOP)/doc/man/*.1 $(STAGING)/doc/man
	cp Makefile.all $(DEST)/Makefile
	@$(call log) "[Generate]" "tar cfz $(ARCHIVE) -C $(STAGING) ."
	tar cfz $(ARCHIVE) -C $(STAGING) .
	rm -fr $(STAGING)

cleanExtra:
	rm -rf $(STAGING)
