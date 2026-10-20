# Builds moon-buggy without autotools (makeinfo, for the manual, is not
# needed to play): make -f asciiverse/Makefile
#
#   CC, CFLAGS, LDFLAGS as usual; CURSES_LIBS (default -lncurses)
#   EXE=.exe for Windows; BUILD, the folder for objects (default build)
#   SED: GNU sed, which img.sed needs (default sed; gsed on macOS)

CC ?= cc
CFLAGS ?= -O2
CURSES_LIBS ?= -lncurses
EXE ?=
SED ?= sed
SRCS = main.c mode.c title.c pager.c game.c level.c ground.c buggy.c \
	laser.c meteor.c highscore.c realname.c queue.c vclock.c date.c \
	persona.c signal.c keyboard.c terminal.c cursor.c random.c error.c \
	xmalloc.c xstrdup.c hpath.c
BUILD ?= build
OBJS = $(SRCS:%.c=$(BUILD)/%.o)

moon-buggy$(EXE): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(CURSES_LIBS) -lm

$(BUILD)/%.o: %.c moon-buggy.h buggy.h copying.h asciiverse/config.h
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) $(CPPFLAGS) -DHAVE_CONFIG_H -Iasciiverse -I. -c -o $@ $<

copying.h: COPYING text2c.sed
	$(SED) -f text2c.sed COPYING > $@.tmp && mv $@.tmp $@
buggy.h: car.img img.sed
	$(SED) -n -f img.sed car.img > $@.tmp && mv $@.tmp $@

clean:
	rm -rf $(BUILD) moon-buggy$(EXE) copying.h buggy.h
