"""drive.py SESSION SECONDS [JUMP]: play moon-buggy in a 100x30 tmux pane
(on the tmux server named by $TMUX_SOCKET, else the default one), from
its title screen.

Records the catalog preview (asciiverse/record.sh): asciinema rec runs
moon-buggy inside the tmux session while this script reads the pane and
sends the keys. The road is row 26 and meteors roll along row 25; both
scroll right towards the buggy, whose front wheel is at column 69 on any
terminal 80 or more wide, its wheels on row 25 when on the ground. It
jumps when on the ground with a crater's near edge JUMP columns from the
wheel, and fires at a meteor in range."""
import os, subprocess, sys, time

S, secs = sys.argv[1], float(sys.argv[2])
JUMP = int(sys.argv[3]) if len(sys.argv) > 3 else 2
WHEEL, ROAD, ROCKS = 69, 26, 25


TMUX = ["tmux"] + (["-L", os.environ["TMUX_SOCKET"]] if os.environ.get("TMUX_SOCKET") else [])


def key(*k):
    subprocess.run(TMUX + ["send-keys", "-t", S, *k])


def screen():
    out = subprocess.run(TMUX + ["capture-pane", "-p", "-t", S],
                         capture_output=True, text=True).stdout
    return [r.ljust(100) for r in (out.split("\n") + [""] * 30)[:30]]


def crater_distance(rows):
    """Columns from the buggy's rear wheel to the nearest crater's near
    edge, or None."""
    road = rows[ROAD]
    for x in range(WHEEL - 1, -1, -1):
        if road[x] == " ":
            return WHEEL - x
    return None


key("Space")
t0, jumps, shots = time.time(), 0, 0
while time.time() - t0 < secs:
    rows = screen()
    if "new game" in rows[28] or "enter your name" in rows[28]:
        break
    d = crater_distance(rows)
    on_ground = rows[ROCKS][WHEEL - 1] == "("
    if d is not None and d <= JUMP and on_ground:
        key("Space"); jumps += 1
        time.sleep(0.1)
    elif any(c in "Oo," for c in rows[ROCKS][WHEEL - 30:WHEEL - 1]):
        key("a"); shots += 1
    time.sleep(0.03)
print(f"time {time.time() - t0:.1f} jumps {jumps} shots {shots}", file=sys.stderr)
