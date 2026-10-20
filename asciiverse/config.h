/* config.h for the Asciiverse build (asciiverse/Makefile), in place of
 * the one configure writes; the same settings configure finds on Linux
 * and macOS, with the Windows differences under _WIN32.  */

#define PACKAGE "moon-buggy"
#define VERSION "1.1.0"

#define CURSES_HEADER <curses.h>
#define RETSIGTYPE void

#define HAVE_ERRNO_H 1
#define HAVE_LOCALE_H 1
#define HAVE_SETLOCALE 1
#define HAVE_FTRUNCATE 1
#define HAVE_GETOPT_H 1
#define HAVE_GETOPT_LONG 1

#ifndef _WIN32
#define HAVE_TERMIOS_H 1
#define HAVE_SETREUID 1
#endif

#if !defined(__APPLE__) && !defined(_WIN32)
#define _XOPEN_SOURCE 1
#endif
