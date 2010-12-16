#	Makefile for the MPR distributionss
#
#	Copyright (c) Embedthis Software LLC, 2003-2010. All Rights Reserved.
#

include			.makedep

STAGING			:= $(PWD)/staging
TDIR			:= $(STAGING)/mpr-$(BLD_VERSION)
REL_DIR			:= $(BLD_OUT_DIR)/releases
DIST_VER_NAME	:= mpr-$(BLD_VERSION)-dist.tgz
DIST_PERM_NAME	:= mpr-dist.tgz
DIST_VER		:= $(REL_DIR)/$(DIST_VER_NAME)
DIST_PERM		:= $(REL_DIR)/$(DIST_PERM_NAME)
FLAT_VER_NAME	:= mpr-$(BLD_VERSION)-flat.tgz
FLAT_PERM_NAME	:= mpr-flat.tgz
FLAT_VER		:= $(REL_DIR)/$(FLAT_VER_NAME)
FLAT_PERM		:= $(REL_DIR)/$(FLAT_PERM_NAME)
DEST			:= $(TDIR)/src/deps/mpr
INC_DEST		:= $(TDIR)/src/include
DOC_DEST		:= $(TDIR)/doc
TARGETS			+= $(patsubst %,$(INC_DEST)/%, mpr.h mprSsl.h)
TARGETS			+= $(patsubst %,$(DEST)/%, mprLib.c makerom.c mprSsl.c angel.c)

#
#	Order of headers matters
#
HEADERS			+= $(patsubst %,../src/include/%, mprOs.h mprTune.h mpr.h mprTest.h)
MPR_SOURCES 	+= ../src/mprMem.c $(shell find ../src/*.c ../src/deps/*.c | egrep -v 'angel|mprMem')
SSL_SOURCES 	+= ../src/include/mprSsl.h $(wildcard ../src/ssl/*.c) 

dist packageExtra: prep $(TARGETS) $(DIST_VER) $(FLAT_VER) cleanup

prep:
	rm -fr $(STAGING) $(DIST_VER) $(DIST_PERM) $(FLAT_VER) $(FLAT_PERM)
	mkdir -p $(DEST) $(INC_DEST) $(DOC_DEST)/man $(DOC_DEST)/api $(REL_DIR)

$(INC_DEST)/mpr.h: Makefile $(HEADERS)
	TITLE="$(BLD_NAME) Header" \
	dist $(HEADERS) | egrep -v '#include.*mpr|#inc.*config.h|#inc.*ucp.*\.h|#inc.*pcre' >$(INC_DEST)/mpr.h

$(INC_DEST)/mprSsl.h: Makefile $(HEADERS)
	cp ../src/include/mprSsl.h $(INC_DEST)/mprSsl.h

$(DEST)/mprLib.c: Makefile $(HEADERS) $(MPR_SOURCES)
	echo -e '#include "mpr.h"' >$(DEST)/mprLib.c
	TITLE="$(BLD_NAME) Library Source" \
	dist $(MPR_SOURCES) | egrep -v '#inc.*mpr' >>$(DEST)/mprLib.c

$(DEST)/makerom.c: Makefile ../src/utils/makerom.c
	cp ../src/utils/makerom.c $(DEST)/makerom.c

$(DEST)/mprSsl.c: Makefile ../src/include/mprSsl.h $(HEADERS) $(SSL_SOURCES)
	echo -e '#include "mpr.h"' >$(DEST)/mprSsl.c
	TITLE="$(BLD_NAME) SSL Library Source" \
	dist $(SSL_SOURCES) | egrep -v '#inc.*mpr' >>$(DEST)/mprSsl.c

$(DEST)/angel.c: Makefile ../src/angel.c
	cp ../src/angel.c $(DEST)/angel.c

$(DIST_VER): 
	cp $(BLD_TOP)/doc/api/mprBare.html $(TDIR)/doc/api
	cp $(BLD_TOP)/doc/man/*.1 $(TDIR)/doc/man
	cp Makefile.dist $(DEST)/Makefile
	@$(call log) "[Generate]" "tar cfz $(DIST_VER) -C $(STAGING) ."
	tar cfz $(DIST_VER) -C $(STAGING) .
	cd $(REL_DIR) ; ln -s $(DIST_VER_NAME) $(DIST_PERM_NAME)

flat $(FLAT_VER):
	find $(TDIR)/src -type f | xargs -I % mv % $(TDIR) 
	rm -fr $(TDIR)/doc $(TDIR)/src $(TDIR)/Makefile
	$(call log) "[Generate]" "tar cfz $(FLAT_VER) -C $(STAGING) ."
	tar cfz $(FLAT_VER) -C $(STAGING) .
	cd $(REL_DIR) ; ln -s $(FLAT_VER_NAME) $(FLAT_PERM_NAME)

cleanup:
	rm -rf $(STAGING)

cleanExtra:
	rm -rf $(STAGING) $(REL_DIR)/*.tgz
