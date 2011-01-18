#
#	Makefile for the MPR unit test source code
#
#	Copyright (c) Embedthis Software LLC, 2003-2011. All Rights Reserved.
#

BUILD_DEPTH	?= 2

include 	.makedep

TARGETS		= $(BLD_BIN_DIR)/benchMpr$(BLD_EXE) $(BLD_BIN_DIR)/testMpr$(BLD_EXE) $(BLD_BIN_DIR)/runProgram$(BLD_EXE)
OBJECTS     = $(patsubst %.c,$(BLD_OBJ_DIR)/%$(BLD_OBJ),$(wildcard test*.c))

compileTargets:	$(TARGETS)

$(BLD_BIN_DIR)/testMpr$(BLD_EXE): $(OBJECTS) $(BLD_LIB_DIR)/libmpr$(BLD_LIB)
	bld --exe $(BLD_BIN_DIR)/testMpr$(BLD_EXE) --search "$(BLD_SSL_LIBPATHS)" --libs "mpr $(BLD_SSL_LIBS)" $(OBJECTS)

$(BLD_BIN_DIR)/runProgram$(BLD_EXE): runProgram.c $(BLD_LIB_DIR)/libmpr$(BLD_LIB)
	bld --exe $(BLD_BIN_DIR)/runProgram$(BLD_EXE) --libs "$(BLD_MPR_LIBS)" runProgram

$(BLD_BIN_DIR)/benchMpr$(BLD_EXE): $(OBJECTS) $(BLD_LIB_DIR)/libmpr$(BLD_LIB) $(BLD_OBJ_DIR)/benchMpr$(BLD_OBJ)
	bld --exe $(BLD_BIN_DIR)/benchMpr$(BLD_EXE) --search "$(BLD_MPR_LIBPATHS)" --libs "$(BLD_MPR_LIBS)" benchMpr

ifneq	($(BLD_CROSS),1)
testExtra: 
	$(call log) "[Test]" "Basic unit tests" ; \
	$(BLD_BIN_DIR)/testMpr -v -i 2 --depth $(BUILD_DEPTH)
	if [ $(BUILD_DEPTH) -ge 3 ] ; then \
		$(call log) "[Test]" "Light load unit tests" ; \
		$(BLD_BIN_DIR)/testMpr -q -i 20 --depth $(BUILD_DEPTH) ; \
		for t in 2 4 ; do \
			$(call log) "[Test]" "Light thread load tests" ; \
			$(BLD_BIN_DIR)/testMpr -q -i 20 --threads $$t --depth $(BUILD_DEPTH) ; \
		done ; \
	fi ; \
	if [ $(BUILD_DEPTH) -ge 4 ] ; then \
		$(call log) "[Test]" "Longevity load tests" ; \
		$(BLD_BIN_DIR)/testMpr -q -i 100 --depth $(BUILD_DEPTH) ; \
		for t in 8 16 ; do \
			$(call log) "[Test]" "Heavy thread load tests" ; \
			$(BLD_BIN_DIR)/testMpr -q -i 100 --threads $$t --depth $(BUILD_DEPTH) ; \
		done ; \
	fi
	rm -f $(BLD_TOP)/.testFile.tmp*
	$(call log) "[Test]" "Unit tests complete"

testFinal:
	killall testMpr >/dev/null 2>&1 ; true

leakTestExtra:
	if type valgrind >/dev/null 2>/dev/null ; then \
		$(call log) "[Run]" "Memory leak tests" ; \
		leakTest --name $(BLD_PRODUCT).mpr --cmd "testMpr -i 1 -v" ; \
	fi

benchExtra:
	echo "# Benchmarking $(BLD_PRODUCT).mpr"
	./benchMpr$(BLD_EXE) -T 0 | tee ../../images/benchmarks/mpr.csv ; \
	./benchMpr$(BLD_EXE) -T 20 -e | tee -a ../../images/benchmarks/mpr.csv
endif

cleanExtra:
	rm -f benchMpr$(BLD_EXE) trace*.txt *.txt.old testMpr$(BLD_LIB)

#
#   Local variables:
#   tab-width: 4
#   c-basic-offset: 4
#   End:
#   vim: sw=4 ts=4 noexpandtab
#
