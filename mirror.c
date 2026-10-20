/* mirror.c - draw the moon's surface mirrored, so the buggy drives right
 *
 * The game always runs from right to left; with `mirrored' set, the
 * drawing on the `moon' window is flipped left to right on the way to
 * the screen: columns run from the other edge, strings are reversed and
 * characters that point one way are swapped for their mirror images.  */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>

#include "moon-buggy.h"


int  mirrored;

static int
flip (int c)
/* The mirror image of the character C.  */
{
  switch (c) {
  case '(':  return ')';
  case ')':  return '(';
  case '/':  return '\\';
  case '\\':  return '/';
  case '<':  return '>';
  case '>':  return '<';
  case '[':  return ']';
  case ']':  return '[';
  case '{':  return '}';
  case '}':  return '{';
  default:  return c;
  }
}

void
moon_addch (int y, int x, int c)
/* Draw the character C at column X of row Y of the moon's surface.  */
{
  if (mirrored)
    mvwaddch (moon, y, COLS-1-x, flip (c));
  else
    mvwaddch (moon, y, x, c);
}

void
moon_addnstr (int y, int x, const char *str, int n)
/* Draw at most N characters of STR from column X of row Y on; with N
 * negative, all of it.  STR need not end in a null within N characters
 * (the ground's rows do not).  */
{
  int  i;

  if (! mirrored) {
    mvwaddnstr (moon, y, x, str, n);
    return;
  }
  for (i=0; (n < 0 || i < n) && str[i]; ++i) {
    if (COLS-1-(x+i) < 0)  break;
    mvwaddch (moon, y, COLS-1-(x+i), flip ((unsigned char)str[i]));
  }
}

void
moon_addstr (int y, int x, const char *str)
{
  moon_addnstr (y, x, str, -1);
}

void
moon_addtext (int y, int x, const char *str)
/* Draw the text STR at column X of row Y, as words: mirrored, it moves
 * to the other side but still reads from left to right.  */
{
  if (mirrored) {
    x = COLS - x - strlen (str);
    if (x < 0)  x = 0;
  }
  mvwaddstr (moon, y, x, str);
}
