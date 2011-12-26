Embedthis Multithreaded Portable Runtime (MPR)
==============================================

To build:
<pre>
    ./configure
    make
</pre>

Key Files and Directories

* LICENSE.TXT           - Product license
* build                 - Build tools and scripts
* configure             - Primary build configuration script
* doc                   - Product documentation
* package               - Packaging scripts
* projects              - IDE project files to build the product
* releases              - Release packages created via make package
* samples               - Sample code
* src                   - Product source code

* out/inc/buildConfig.h - Configure settings
* out/bin               - All created binaries. On windows, DLLs go here also.
* out/lib			    - Static and shared libraries
* out/obj               - Compiled source objects
* out/include           - Directory for generated include files

See Also
--------

LICENSE.TXT -- License details.

--------------------------------------------------------------------------------
Copyright (c) 2003-2012 Embedthis Software, LLC. All Rights Reserved.
Embedthis and Ejscript are trademarks of Embedthis Software, LLC. Other 
brands and their products are trademarks of their respective holders.

See LICENSE.TXT for software license details.

