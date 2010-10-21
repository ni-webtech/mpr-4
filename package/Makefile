#
#	Makefile for the combo MPR distribution
#
#	Copyright (c) Embedthis Software LLC, 2003-2010. All Rights Reserved.
#

include			.makedep

# COMPILE			=
STAGING		:= $(PWD)/staging
DIR			:= $(STAGING)/mpr-$(BLD_VERSION)
REL_DIR		:= ../releases
RELEASE		:= mpr-$(BLD_VERSION)-combo.tgz
LATEST		:= mpr-combo.tgz
ARCHIVE		:= $(REL_DIR)/$(RELEASE)
DEST		:= $(DIR)/src/deps/mpr
INC_DEST	:= $(DIR)/src/include
DOC_DEST	:= $(DIR)/doc
TARGETS		+= $(patsubst %,$(INC_DEST)/%, mpr.h mprSsl.h)
TARGETS		+= $(patsubst %,$(DEST)/%, mprLib.c makerom.c mprSsl.c angel.c)

#
#	Order of headers matters
#
HEADERS			+= $(patsubst %,../src/include/%, mprOs.h mprTune.h mpr.h mprTest.h)
MPR_SOURCES 	+= ../src/mprAlloc.c $(shell find ../src/*.c ../src/deps/*.c | egrep -v 'angel|mprAlloc')
SSL_SOURCES 	+= ../src/include/mprSsl.h $(wildcard ../src/ssl/*.c) 

combo packageExtra: prep $(TARGETS) $(ARCHIVE) cleanExtra

prep:
	rm -fr $(DIR)
	mkdir -p $(DEST) $(INC_DEST) $(DOC_DEST)/man $(DOC_DEST)/api

$(INC_DEST)/mpr.h: Makefile $(HEADERS)
	combo $(HEADERS) | egrep -v '#include.*mpr|#inc.*config.h|#inc.*ucp.*\.h|#inc.*pcre' >$(INC_DEST)/mpr.h

$(INC_DEST)/mprSsl.h: Makefile $(HEADERS)
	cp ../src/include/mprSsl.h $(INC_DEST)/mprSsl.h

$(DEST)/mprLib.c: Makefile $(HEADERS) $(MPR_SOURCES)
	echo -e '#define BLD_MPRLIB 1\n#define MPR_IN_ALLOC 1\n#include "mpr.h"' >$(DEST)/mprLib.c
	combo $(MPR_SOURCES) | egrep -v '#inc.*mpr' >>$(DEST)/mprLib.c

$(DEST)/makerom.c: Makefile ../src/utils/makerom.c
	cp ../src/utils/makerom.c $(DEST)/makerom.c

$(DEST)/mprSsl.c: Makefile ../src/include/mprSsl.h $(HEADERS) $(SSL_SOURCES)
	combo $(HEADERS) $(SSL_SOURCES) | egrep -v '#inc.*mpr' >$(DEST)/mprSsl.c

$(DEST)/angel.c: Makefile ../src/angel.c
	cp ../src/angel.c $(DEST)/angel.c

$(ARCHIVE): Makefile $(TARGETS)
	cp $(BLD_TOP)/doc/api/mprBare.html $(DIR)/doc/api
	cp $(BLD_TOP)/doc/man/*.1 $(DIR)/doc/man
	cp Makefile.combo $(DEST)/Makefile
	rm -f $(REL_DIR)/$(RELEASE) $(REL_DIR)/$(LATEST)
	@$(call log) "[Generate]" "tar cfz $(ARCHIVE) -C $(STAGING) ."
	tar cfz $(REL_DIR)/$(RELEASE) -C $(STAGING) .
	cd $(REL_DIR) ; ln -s $(RELEASE) $(LATEST)

cleanExtra:
	rm -rf $(STAGING)
