#
#	Makefile -- API Documentation makefile
#
#	Copyright (c) Embedthis Software LLC, 2003-2012. All Rights Reserved.
#

include		.makedep

gen: prep mpr.html

prep:
	touch Makefile

mpr.html: Makefile
	make -C mpr gen

tidy:
	tidy -q -config tidy.conf mpr.html

#
#   Local variables:
#   tab-width: 4
#   c-basic-offset: 4
#   End:
#   vim: sw=4 ts=4 noexpandtab
#
