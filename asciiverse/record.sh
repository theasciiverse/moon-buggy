#!/bin/sh
# Records asciiverse/moon-buggy.cast, the catalog preview, at 100x30:
#
#   asciiverse/record.sh path/to/moon-buggy [seconds]
#
# asciinema rec runs moon-buggy (by name, from a throwaway folder on PATH,
# with a throwaway home) in a detached 100x30 tmux session; drive.py
# starts a game from the title and plays it. The take ends while the
# buggy is still driving: the game is stopped with SIGTERM, so it never
# reaches game over, whose name prompt offers the passwd entry's real
# name as the default.
set -eu
cd "$(dirname "$0")/.."
bin=$(cd "$(dirname "$1")" && pwd)/$(basename "$1")
secs=${2:-50}
out=$PWD/asciiverse/moon-buggy.cast
S=moon-buggy-preview-$$
home=$(mktemp -d)
trap 'tmux -L "$S" kill-server 2>/dev/null; rm -rf "$home"' EXIT
mkdir "$home/bin"
cp "$bin" "$home/bin/moon-buggy"
tmux -L "$S" new-session -d -s "$S" -x 100 -y 30 -c "$home" \
  "env -u TMUX HOME=$home XDG_DATA_HOME= PATH=$home/bin:$PATH TERM=xterm-256color LANG=C.UTF-8 USER=player LOGNAME=player \
   asciinema rec --overwrite -q --window-size 100x30 --idle-time-limit 2 --title moon-buggy \
     --capture-env TERM,LANG -c moon-buggy $out"
sleep 3   # the title screen
TMUX_SOCKET=$S python3 asciiverse/drive.py "$S" "$secs"
# stop the moon-buggy under this pane (not any other one running)
pids=$(tmux -L "$S" display-message -p -t "$S" '#{pane_pid}')
for _ in 1 2 3; do
  pids="$pids $(for p in $pids; do pgrep -P "$p" || true; done)"
done
stopped=
for p in $(echo $pids | tr ' ' '\n' | sort -u); do
  if [ "$(ps -o comm= -p "$p" 2>/dev/null)" = moon-buggy ]; then
    echo "stopping moon-buggy, pid $p" >&2; kill -TERM "$p" && stopped=1
  fi
done
[ -n "$stopped" ] || { echo "moon-buggy was not found to stop" >&2; exit 1; }
for _ in 1 2 3 4 5 6 7 8 9 10; do
  tmux -L "$S" has-session -t "$S" 2>/dev/null || break
  sleep 0.5
done
# end on the game: drop the exit (the screen restored, "GAME ABORTED")
python3 - "$out" <<'PY'
import json, sys
lines = open(sys.argv[1]).read().splitlines()
keep = [lines[0]]
for line in lines[1:]:
    ev = json.loads(line)
    if ev[1] == "x" or (ev[1] == "o" and "\x1b[?1049l" in ev[2]):
        break
    keep.append(line)
open(sys.argv[1], "w").write("\n".join(keep) + "\n")
PY
if grep -q "enter your name" "$out"; then
  echo "the take reached the name prompt (passwd real name): not usable" >&2
  exit 1
fi
echo "recorded $out"
