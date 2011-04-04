#
#	Makefile -- MPR API Documentation
#
#	Copyright (c) Embedthis Software LLC, 2003-2011. All Rights Reserved.
#

STAGING		:= staging

include		.makedep

gen:
	make SKIP_CLEAN=1 -C ../../../package dist
	rm -fr ./$(STAGING) xml html
	mkdir -p ./$(STAGING) xml html
	tar xf $(BLD_OUT_DIR)/releases/$(BLD_PRODUCT)-dist.tgz -C $(STAGING)
	mv $(STAGING)/mpr-$(BLD_VERSION)/* $(STAGING)
	rmdir $(STAGING)/mpr-$(BLD_VERSION)
	$(call log) "[Generate]" "doxygen mpr.spec"
	doxygen mpr.spec
	$(call log) "[Transform]" "gendoc"
	ejs $(BLD_TOOLS_DIR)/gendoc.es --bare --title "MPR - Multithreaded Portable Runtime Native API" --tags ../mpr.tags \
		xml/mpr_8h.xml xml/group* xml/struct_*.xml >../mprBare.html
	ejs $(BLD_TOOLS_DIR)/gendoc.es --title "MPR - Multithreaded Portable Runtime Native API" --tags ../mpr.tags \
		xml/mpr_8h.xml xml/group* xml/struct_*.xml >../mpr.html
	-fr ./$(STAGING) xml html

cleanExtra:
	rm -rf xml html ../mpr.tags

clobberExtra:
	rm -f ../mpr.html ../mprBare.html
	rm -fr ./$(STAGING)

#
#   Local variables:
#   tab-width: 4
#   c-basic-offset: 4
#   End:
#   vim: sw=4 ts=4 noexpandtab
#
