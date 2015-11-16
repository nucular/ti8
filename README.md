![A fancy screenshot](https://github.com/nucular/ti8/raw/master/screenshot.png)

TI8
===

A CHIP-8 emulator for TI calculators.  
Project status: works

Usage
-----

1. Put it on your calculator through TI Graph-Link.
2. Run your CHIP-8 programs through tipack
   (`tipack myprog.ch8 folder\myprog CH8 myprog.v2y`).
3. Put them on your calculator too.
4. Run `ti8("folder\myprog")` to launch the program.

Keys
----

![Another fancy screenshot](https://github.com/nucular/ti8/raw/master/keys.png)

- Numbers/x/-/+/Enter: Key 0-F, see picture
  - The COSMAC VIP had an awkward keyboard layout
- Escape: Exit emulator
- Apps: Pause/resume execution
- Mode: Enter/leave stepthrough mode
  - Apps: Execute next instruction
  - Up/Down: Select register, I or program counter
  - Left/Right: Increment/decrement selected register by 1
  - Clear: Clear selected register
  - F1-F8: Jump to register number

Building
--------

1. Get [TIGCC](http://tigcc.ticalc.org) and Make
2. `make ti8`

Building tipack is trivial, the Makefile will default to GCC but you can really
just run it through your favourite C compiler like you'd do with any single-file
project.

TODO
----

- Make the keyboard demo not crash because of invalid reallocation (I have no
    fucking clue why this happens)
- Fix the various other unknown issues
- Add Super-Chip support
