/* hpath.c - the central highscore file location
 *
 * Copyright 1999  Jochen Voss  */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "moon-buggy.h"

#ifdef _WIN32
#define mkdir(path, mode) mkdir (path)
#endif


const char *score_dir;

#ifdef SCORE_DIR

void
init_score_dir (void)
{
  score_dir = SCORE_DIR;
}

#else

static void
make_dirs (char *path)
/* Create the folder PATH and any missing parents, as "mkdir -p" does.  */
{
  char *p;

  for (p = path+1; *p; ++p) {
    if (*p == '/') {
      *p = '\0';
      mkdir (path, 0755);
      *p = '/';
    }
  }
  mkdir (path, 0755);
}

void
init_score_dir (void)
/* Without a shared score folder (SCORE_DIR), keep the score file in the
 * player's data folder: $XDG_DATA_HOME/moon-buggy, else
 * ~/.local/share/moon-buggy (%APPDATA%\moon-buggy on Windows).  Empty
 * variables count as unset.  Without either, the score file is
 * ~/.mbscore as before.  */
{
#ifdef _WIN32
  const char *base = getenv ("APPDATA");
  const char *home = NULL;
#else
  const char *base = getenv ("XDG_DATA_HOME");
  const char *home = getenv ("HOME");
#endif
  char *dir;

#ifdef _WIN32
  if (base && base[0]) {
#else
  if (base && base[0] == '/') {
#endif
    dir = xmalloc (strlen (base) + sizeof "/moon-buggy");
    strcpy (dir, base);
  } else if (home && home[0]) {
    dir = xmalloc (strlen (home) + sizeof "/.local/share/moon-buggy");
    strcpy (dir, home);
    strcat (dir, "/.local/share");
  } else {
    return;
  }
  strcat (dir, "/moon-buggy");
  make_dirs (dir);
  score_dir = dir;
}

#endif
