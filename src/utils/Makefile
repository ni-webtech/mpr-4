#
#	Makefile for the MPR utilities
#
#	Copyright (c) Embedthis Software LLC, 2003-2010. All Rights Reserved.
#

include 	.makedep

ifeq ($(BUILD_NATIVE_OR_COMPLETE_CROSS),1)
	TARGETS	+= $(BLD_BIN_DIR)/makerom$(BLD_EXE)
	TARGETS	+= $(BLD_BIN_DIR)/charGen$(BLD_EXE)
endif

compileTargets: $(TARGETS)

$(BLD_BIN_DIR)/makerom$(BLD_EXE): $(BLD_OBJ_DIR)/makerom$(BLD_OBJ) $(BLD_LIB_DIR)/libmpr$(BLD_LIB)
	@bld --exe $(BLD_BIN_DIR)/makerom$(BLD_EXE) --search "$(BLD_MPR_LIBPATHS)" --libs "$(BLD_MPR_LIBS)" makerom

$(BLD_BIN_DIR)/charGen$(BLD_EXE): $(BLD_OBJ_DIR)/charGen$(BLD_OBJ)
	@bld --exe $(BLD_BIN_DIR)/charGen$(BLD_EXE) --search "$(BLD_MPR_LIBPATHS)" --libs "$(BLD_MPR_LIBS)" charGen

