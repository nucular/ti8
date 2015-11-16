#ifndef CONF_H
#define CONF_H

// Try to emulate the original CHIP8, not other emulators
#define EMU_REALISTIC
// Scale the emulator screen
#define SCREEN_SCALE2X
// Center the emulator screen
#define SCREEN_CENTER

// Program Comments
#define COMMENT_STRING "A CHIP-8 emulator for TI calculators"
#define COMMENT_PROGRAM_NAME "ti8"
#define COMMENT_VERSION_STRING "0.1"
#define COMMENT_VERSION_NUMBER 0,1,0,0 /* major, minor, revision, subrevision */
#define COMMENT_AUTHORS "nucular"

// Supported calculators and AMS
#define USE_V200
#define USE_TI92PLUS
#define MIN_AMS 200

// Opimization flags
#define SAVE_SCREEN
#define OPTIMIZE_ROM_CALLS
#define OPTIMIZE_CALC_CONSTS

#endif
