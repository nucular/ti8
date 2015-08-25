TI8
===

A CHIP-8 emulator for TI calculators.  
Project status: works (screen scaling crashes though)

Usage
-----

1. Put it on your calculator through TI Graph-Link.
2. Run your CHIP-8 programs through tipack
   (`tipack myprog.ch8 folder\myprog CH8 myprog.v2y`).
3. Put them on your calculator too.
4. Run `ti8("folder\myprog")` to launch the program.

Building
--------

1. Get [TIGCC](http://tigcc.ticalc.org) and Make
2. `make ti8`

Building tipack is trivial, the Makefile will default to GCC but you can really
just run it through your favourite C compiler like you'd do with any single-file
project.
