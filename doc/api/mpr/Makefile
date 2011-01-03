#
#	Makefile -- MPR API Documentation
#
#	Copyright (c) Embedthis Software LLC, 2003-2011. All Rights Reserved.
#

include		.makedep

doc:
	make SKIP_CLEAN=1 -C ../../../package dist
	rm -fr xml html
	mkdir -p xml html
	doxygen mpr.spec
	ejs $(BLD_TOOLS_DIR)/gendoc.es --bare --title "MPR - Multithreaded Portable Runtime Native API" --tags ../mpr.tags \
		xml/mpr_8h.xml xml/group* xml/struct_*.xml >../mprBare.html
	ejs $(BLD_TOOLS_DIR)/gendoc.es --title "MPR - Multithreaded Portable Runtime Native API" --tags ../mpr.tags \
		xml/mpr_8h.xml xml/group* xml/struct_*.xml >../mpr.html
	make -C ../../../package cleanup

cleanExtra:
	rm -rf xml html ../mpr.tags

clobberExtra:
	rm -f ../mpr.html ../mprBare.html

#
#   Local variables:
#   tab-width: 4
#   c-basic-offset: 4
#   End:
#   vim: sw=4 ts=4 noexpandtab
#
