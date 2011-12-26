#
#	Makefile -- Man page makefile
#
#	Copyright (c) Embedthis Software LLC, 2003-2012. All Rights Reserved.
#

include		.makedep

compileExtra: manager.txt manager.html makerom.txt makerom.html

%.txt: %.1
	man ./$*.1 | sed 's/.\//g' >$*.txt

%.html: %.1
	man ./$*.1 | man2html -u >$*.html

%.txt: %.3
	man ./$*.3 | sed 's/.\//g' >$*.txt

%.html: %.3
	man ./$*.3 | man2html -u >$*.html

cleanExtra:
	rm -f manager.txt

#
#   Local variables:
#   tab-width: 4
#   c-basic-offset: 4
#   End:
#   vim: sw=4 ts=4 noexpandtab
#
