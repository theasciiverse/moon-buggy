# Changes from upstream

This is the Asciiverse edition of moon-buggy, Jochen Voss' game of
driving a buggy across the moon (C and curses, 1998 on), built from his
own repository, github.com/seehuhn/moon-buggy (the `upstream` remote),
at `5683e69` (2026-10-13), one README commit after the v1.1.0 release
(2025-11-17). The history goes back to 1998 through his Subversion years.
The `asciiverse` branch carries the changes below.

## Licence

The program's notices (README, `moon-buggy -V`, the title screen, the
manual) say moon-buggy may be redistributed "under the terms of the GNU
General Public License" and name no version. COPYING is the GPL version 3
text since the move to git in 2011; release 1.0.51 (2006) shipped the
version 2 text, and moon-buggy.lsm still says "GPL 2.0". When a program
names no version, the GPL lets the recipient choose any version ever
published (section 14 of version 3, section 9 of version 2), so the
catalog gives `GPL-1.0-or-later`. Our changes are under the same terms.

## Debian

Debian maintains moon-buggy (1.0.51-17 in unstable,
salsa.debian.org/debian/moon-buggy). None of its four patches is applied:
the GCC 15 fix is in upstream since February 2025 (Robert Scheck's
`c929f2c`, the same change); the others change the install rule (not
used here), convert the TODO file to UTF-8, and add a pause key that
Debian itself leaves switched off, as its author wrote that it "violates
the spirit of the game".

## Code

- **A score file name's terminating zero stays inside the entry.** Names
  are read from the score file with up to 40 characters and strncat adds
  the zero after them, one byte past the 40-byte field; names entered in
  the game fill all 40 bytes too. The field is one byte longer (found by
  the compiler's -Wstringop-overflow).
- **The terminal is restored once when it has gone away.** When the
  terminal window is closed, restoring its settings fails, fatal()
  restores them again on its way out, and the two called each other until
  the stack ran out (found by an AddressSanitizer soak). They are marked
  as restored before trying, so the game exits with the first error.
- **The score file is in the player's data folder** when the build has
  no shared score folder: `$XDG_DATA_HOME/moon-buggy/mbscore`, else
  `~/.local/share/moon-buggy/mbscore` (`%APPDATA%\moon-buggy` on
  Windows), made when missing; empty variables count as unset. Upstream
  builds always have a shared folder (configure's `SCORE_DIR`, normally
  setgid under /var/games), and theirs work as before.
- **Windows** (MinGW and PDCurses), all behind `_WIN32`: the wait for the
  next key or game tick uses curses' key timeout instead of select() on
  stdin, in steps of at most 0.1 s so Ctrl-C is noticed, and a resized
  console (KEY_RESIZE) lays the screen out again as SIGWINCH does; signal()
  for SIGINT and SIGTERM; no setuid personas, passwd entries, fcntl locks
  or "mesg n"; the name prompt offers `%USERNAME%`.

- **`-r`, `--right`: drive to the right** (David's request, 2026-10-20),
  as in Irem's Moon Patrol. The game runs as before; everything drawn on
  the moon's surface is mirrored on its way to the screen (mirror.c):
  columns from the other edge, strings reversed, `( ) / \ < > [ ] { }`
  swapped, so the buggy faces right and its wheels turn the right way;
  "GAME OVER" moves across but reads as before. Off by default. Listed in
  `--help`, the man page and the manual; ASan-clean over a soak with it.

Upstream's play is unchanged: the screen already fills the terminal and
follows resizes, and the buggy sits 12 columns from the right of the
first 80 (column 68 on any wider terminal), as it always has.

## Packaging

- `asciiverse/Makefile` and `asciiverse/config.h` build without autotools
  (upstream's build needs makeinfo for the manual, which is not shipped
  here). GNU sed makes the buggy's frames from car.img, so the Makefile
  takes `SED` (gsed on macOS).
- `.github/workflows/release.yml`: static musl Linux (Alpine, x86-64 and
  ARM64), macOS (Intel and Apple silicon, system ncurses) and Windows
  x86-64 (MinGW, PDCurses 3.9 wincon), on our tags (`*-av.*`).
  `asciiverse/package.sh` packs `moon-buggy`, COPYING, README and this
  file.
- `asciiverse/record.sh` records the catalog preview,
  `asciiverse/moon-buggy.cast`, with `asciiverse/drive.py`, which reads
  the screen through tmux, jumps the craters and shoots the meteors. The
  take ends while the buggy is still driving: the game's name prompt for
  a high score offers the passwd entry's real name, so the recorder stops
  the game before game over and checks that no prompt was recorded.

## Checked

Linux: native and static (Alpine) builds played; scores saved in the data
folder; the terminal closed mid-game (the old crash) exits cleanly;
AddressSanitizer and UBSan clean over five random-key soaks of two to
two and a half minutes, with resizes from 20x8 to 200x60, the game quit
and started again 93 times, two of them on the finished code. Windows: cross-built and
played under Wine to game over, a high score entered and saved in
`%APPDATA%\moon-buggy`; a resized console under Wine inside tmux did not
redraw properly, which may be Wine's console, so resizing on real Windows
is unchecked. macOS: not built here.
