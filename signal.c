/* signal.c - signal handling
 *
 * Copyright 1999, 2000, 2001  Jochen Voss.  */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <assert.h>

#include "moon-buggy.h"
#include "darray.h"


struct sig_info {
  int  signum;
  volatile  sig_atomic_t  pending;
  void (*handler) (int);
};
static struct {
  struct sig_info *data;
  int  slots, used;
} sig_info_table;

static volatile  sig_atomic_t  signal_arrived;

#ifdef _WIN32

/* Windows has `signal' for SIGINT and SIGTERM, but no signal masks.  */

void
block_all (void)
{
}

void
unblock (void)
{
}

static void
install_signal (int signum, RETSIGTYPE (*handler) (int))
{
  signal (signum, handler);
}

#else /* not _WIN32 */

static  sigset_t  full_set, old_sigset;


void
block_all (void)
/* Block all signals until `unblock' is called.  */
{
  sigprocmask (SIG_BLOCK, &full_set, &old_sigset);
}

void
unblock (void)
/* Undo the effect of `block_all'.  */
{
  sigprocmask (SIG_SETMASK, &old_sigset, NULL);
}

static void
install_signal (int signum, RETSIGTYPE (*handler) (int))
/* Emulate the `signal' function via `sigaction'.  */
{
  struct sigaction  action;
  int  ret;

  action.sa_handler = handler;
  sigemptyset (&action.sa_mask);
  action.sa_flags = 0;
  ret = sigaction (signum, &action, NULL);
  assert (ret == 0);
}

#endif /* not _WIN32 */

static RETSIGTYPE
generic_handler (int signum)
/* Interrupt handlers shouldn't do much.  So we just note that the
 * signal arrived.  */
{
  int  i;

  signal_arrived = 1;
  for (i=0; i<sig_info_table.used; ++i) {
    if (sig_info_table.data[i].signum == signum) {
      sig_info_table.data[i].pending = 1;
      break;
    }
  }
  /* Note: We don't call assert() or sigaction() here because they are
   * not async-signal-safe.  With modern sigaction() and sa_flags=0,
   * handlers persist automatically, so no reinstallation is needed.
   * If the signal isn't found in our table, we simply ignore it. */
}

static void
my_signal (int signum, void (*handler)(int), int ignore_test)
/* Install HANDLER as a new signal handler for signal SIGNUM.  But if
 * IGNORE_TEST is true and SIGNUM was ignored before, do nothing.  */
{
  struct sig_info *info;

#ifndef _WIN32
  if (ignore_test) {
    struct sigaction  action;

    sigaction (signum, NULL, &action);
    if (action.sa_handler == SIG_IGN)  return;
  }
#endif

  DA_ADD_EMPTY (sig_info_table, struct sig_info, info);
  info->signum = signum;
  info->pending = 0;
  info->handler = handler;

  install_signal (signum, generic_handler);
}

/************************************************************
 * signal handlers
 */

static void
termination_handler (int signum)
{
  prepare_for_exit ();
  fprintf (stderr, "GAME ABORTED (signal %d)\n", signum);
  /* Restore default handler so pressing C-c twice exits, even if hung. */
  install_signal (signum, SIG_DFL);
  raise (signum);
}

#ifndef _WIN32
static void
tstp_handler (int signum)
{
  clock_freeze ();
  mode_signal (signum);
  prepare_for_exit ();
  install_signal (SIGTSTP, SIG_DFL);
  raise (SIGTSTP);
}

static void
cont_handler (int signum)
{
  install_signal (SIGTSTP, tstp_handler);

  refresh ();
  prepare_screen ();
  mode_redraw ();
  mode_signal (signum);
  clock_thaw ();
}

#endif /* not _WIN32 */

#ifdef SIGWINCH
static void
winch_handler (int signum)
{
  clock_freeze ();
  delwin (moon);
  delwin (status);
  delwin (message);
  sleep (1);
  endwin ();

  refresh ();
  allocate_windows ();
  hide_cursor ();
  mode_redraw ();
  clock_thaw ();
}
#endif

#ifdef _WIN32
void
screen_resized (void)
/* PDCurses reports a resized console as the key KEY_RESIZE, not as a
 * signal: lay the screen out again for the new size, as `winch_handler'
 * does.  */
{
  clock_freeze ();
  resize_term (0, 0);
  delwin (moon);
  delwin (status);
  delwin (message);
  clear ();
  refresh ();
  allocate_windows ();
  hide_cursor ();
  mode_redraw ();
  clock_thaw ();
}
#endif

/************************************************************
 * The outside-visible entry point
 */

void
initialise_signals (void)
{
  DA_INIT (sig_info_table, struct sig_info);
  my_signal (SIGINT, termination_handler, 1);
  my_signal (SIGTERM, termination_handler, 1);
#ifndef _WIN32
  my_signal (SIGHUP, termination_handler, 1);
  my_signal (SIGCONT, cont_handler, 0);
  my_signal (SIGTSTP, tstp_handler, 0);
#endif
#ifdef SIGWINCH
  my_signal (SIGWINCH, winch_handler, 0);
#endif

#ifndef _WIN32
  sigfillset (&full_set);
#endif
}

int
handle_signals (void)
/* Execute signal actions, for all signals, which occured before.
 * Return 1, if any action was taken.  */
{
  int  res = 0;

  while (signal_arrived) {
    int  i;

    signal_arrived = 0;
    res = 1;
    for (i=0; i<sig_info_table.used; ++i) {
      if (sig_info_table.data[i].pending) {
        sig_info_table.data[i].pending = 0;
        sig_info_table.data[i].handler (sig_info_table.data[i].signum);
      }
    }
  }
  return  res;
}
