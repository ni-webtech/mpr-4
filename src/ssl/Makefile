#
#	Makefile to build the SSL support
#
#	Copyright (c) Embedthis Software LLC, 2003-2012. All Rights Reserved.
#

include 		.makedep

MAKE_IFLAGS		+= $(BLD_OPENSSL_IFLAGS) $(BLD_MATRIXSSL_IFLAGS)
ifeq ($(MACOSX),1)
	MAKE_IFLAGS	+= -Wno-deprecated-declarations
endif

ifeq ($(BLD_FEATURE_SSL),1)
	IMPORTS 	+= $(BLD_OPENSSL_IMPORTS) $(BLD_MATRIXSSL_IMPORTS)
	IMPORTED	+= $(patsubst %,$(BLD_LIB_DIR)/%, $(notdir $(IMPORTS)))

compileExtra:	$(IMPORTED)

$(IMPORTED): $(IMPORTS)
	getlib $^
ifeq ($(BLD_FEATURE_MATRIXSSL),1)
	[ -x $(BLD_TOP)/projects/MACOSX ] && find $(BLD_TOP)/projects/MACOSX -name Debug | \
		while read d ; do cp $(BLD_MATRIXSSL_IMPORTS) $$d ; done
endif
endif

#
#	This should be done for OpenSSL for each new application to generate new DH parameters
#
opensslParams:
	openssl dhparam -out dh512.pem -C -2 512 >dh.c
	openssl dhparam -out dh1024.pem -C -2 1024 >>dh.c

#
#   Local variables:
#   tab-width: 4
#   c-basic-offset: 4
#   End:
#   vim: sw=4 ts=4 noexpandtab
#
