# Design: first-person colour 3D exterior with day/night

Date: 2026-11-26 · Status: draft for Rowan's approval (builder check done)

## Intent
Make the whole outdoor town a live first-person 3D world drawn in full colour inside the terminal, in the
style of Rowan's reference screenshots (pixel-art western town, sunset sky, horse). The player moves with
the arrow keys and does everything else by typing, so it stays a text adventure. Time of day passes and
the town changes from day to dusk to night. The world (map, buildings, props, sprites, sky colours) lives
in plain text files under `data/world/` so the team can add and edit it without touching code. Interiors
stay as the existing location screens for now; they are rebuilt one by one later.

## Goals
- One continuous outdoor map covering every exterior location: Desert Road (east), Main Street with the
  nine README buildings, the Graveyard behind the graves fence, Soda Lake past it, The Line at the far edge
  of the lakebed, the hill trail to the Old Mine entrance (north), and the Dry Well (west end).
- Live first-person view: each arrow key turns (left/right) or steps (up/down) and the frame redraws at
  once; holding a key repeats. Walls, fences, rocks and the well block movement.
- Full 24-bit colour using the upper-half block `▀` (two pixels per cell). The view fills the terminal:
  width = terminal columns, height = (rows − text area) × 2 pixels, re-measured every frame.
- Only changed cells are re-sent each frame; alternate screen buffer and hidden cursor while playing; the
  terminal is always restored on exit (normal quit, end of input, or Ctrl-C).
- Typed interaction: any printable key types into the command line at the bottom; Backspace edits; Enter
  runs it. No single-key shortcuts. Commands: `look`, `mount` / `ride horse`, `dismount`, `enter <place>`
  (or `enter` when at a door), `wait`, `help`, `quit`, plus the existing `take`, `drop`, `inventory`.
- Horse: visible sprite when you are on foot; `mount` only when standing next to it; riding moves twice as
  far per step, raises the eye height and shows ears/mane/reins at the bottom of the view; `dismount`
  leaves the horse where you are. You can't enter a door on horseback ("Dismount first.").
- Day/night: a game clock. Stepping costs 2 minutes, a typed action 5, `wait` 60; turning is free. The
  game starts at 17:30 (riding in at sunset). Sky, sun/moon, stars, fog and overall light follow the clock;
  at night building windows glow warm yellow. Colour keyframes come from `data/world/sky.txt`.
- Zones: the map is divided into named zones that match the existing exterior rooms (desert_road,
  main_street, graveyard, soda_lake, the_line). The status line shows the zone name and the time; `look`
  describes the zone and what you are facing.
- Doors: each building, the mine entrance and the well have a door point. `enter` within 1.5 tiles of a door
  opens that place's existing location screen; `leave` there puts you back outside the door, facing away.
- Fallbacks: if input is not a live terminal (piped scripts, tests), the current typed-command location game
  runs instead. `--plain` draws the 3D view with density characters and no colour.

## Non-goals (later phases)
3D interiors, NPCs and dialogue, story events, the line advancing through town, sound, saving.

## Anti-goals
- No new libraries or build changes: POSIX `termios`/`ioctl` only, C++11, everything under include/src/data/test.
- Art and map files never decide game rules beyond walkable/blocked, door points and zones.
- A crash or Ctrl-C must never leave the terminal in raw mode.
- No Claude attribution.

## Approaches considered
1. **Grid raycaster with variable wall heights, textured floors and billboard sprites (chosen).** It is the
   technique behind the reference screenshot 3, runs fast enough in C++ for a 160×80 frame, and the spike
   frames already match the look Rowan approved.
2. True polygon 3D (triangles, z-buffer). Freer shapes (sloped roofs, round well) but far more code and slower
   in a terminal; not needed for a street of box buildings.
3. Pre-rendered views per position/angle. Simple to draw, but no live movement; rejected by Rowan.

## World files (`data/world/`)
- `town.map` — ASCII grid, one character per tile. Ground: `.` sand, `=` street, `_` boardwalk, `,` dry grass,
  `~` salt flat. Blocking: `#` rock, `F` fence, `O` well. Lower-case letters `a`–`z` are building footprints
  defined in `buildings.txt`. `@` marks the player start, `h` is not used (horse comes from `props.txt`).
- `buildings.txt` — one building per line: letter, id, sign label, height, style
  (planks/adobe/barn/church/fence/rock), base RGB, trim RGB, door tile x,y, facing (N/S/E/W), room id.
- `props.txt` — sprites placed in the world: sprite name, x, y (tile units, decimals allowed), blocking yes/no.
  Includes the horse start, headstones, crosses, cacti, barrels, hitching posts, water trough, wagon.
- `sprites/<name>.txt` — pixel art: a palette block (`B = 122 74 40`) then rows of palette letters; `.` is
  transparent. Width/height in world units on the first line.
- `zones.txt` — named rectangles mapping tile areas to room ids, plus the description used by `look`.
- `sky.txt` — time keyframes (`HH:MM`): sky top RGB, horizon RGB, light level 0–1, sun or moon.
Malformed lines are skipped with a warning on stderr; a missing file falls back to a built-in default so the
game always starts.

## Architecture (new units)
- `Rgb`, `FrameBuffer` (W×H pixels), `TerminalPainter` (frame → half-block escape string; diff against the
  previous frame; plain mode).
- `WorldMap` (tiles, buildings, zones, doors; `isBlocked`, `zoneAt`, `doorNear`), `WorldFiles` (parsers for
  the six formats, each a pure function from text to data, plus file loading).
- `Sprite`/`Prop` (pixel art + placement), `Camera` (x, y, angle, eye height; `turn`, `step` with sliding
  collision), `Horse` (position, mounted).
- `GameClock` (minutes, `advance`, `timeOfDay`, `light`), `SkyModel` (interpolates `sky.txt` keyframes).
- `Raycaster` (renders sky, sun/moon/stars, mountains, floor, walls with procedural textures, lit windows,
  sprites with a depth buffer, the riding overlay, into a FrameBuffer). Pure: same inputs, same pixels.
- `KeyDecoder` (byte stream → Up/Down/Left/Right/Char/Backspace/Enter/Eof; pure), `RawTerminal` (RAII
  raw mode, alternate screen, size query, signal-safe restore), `IKeyInput` + `TerminalKeyInput`.
- `ExteriorGame` (the explore loop: keys → camera/horse/clock, typed line → commands, door → existing
  location `Game` screen and back). Existing `Game`, rooms, commands and scenes stay.

## Testing
Every unit above except `RawTerminal`/`TerminalKeyInput` (real terminal I/O) and `main` is tested with
GoogleTest: parsers with good and broken text, collision and sliding, clock and sky interpolation, raycaster
pixel checks (sky above the horizon, a wall in front, sprite hidden behind a wall, lit windows only at night),
painter output and diffing, key decoding including split escape sequences, and `ExteriorGame` driven by a
fake key source and fake display. Coverage > 90%, valgrind clean, cppcheck clean.

## Performance target
A full redraw of a 160×80 frame renders in under 30 ms on the lab machines; diffed output keeps typical frames
well under the full ~250 KB. Measured, not assumed, before claiming it.

## Settled questions (builder check; all are my calls unless noted)
1. **Exterior ↔ existing game.** One `Player`, `GameState` and `World` are shared. The camera's zone sets the
   player's current room, so typed commands act on that room; zones are labels plus "current room" (no new
   room effects fire, since rooms have none yet). `enter` checks the target room's `canEnter`, so locked
   rooms stay locked. Inside a location screen the existing exits work between interior rooms (lobby ↔
   hotel room ↔ cellar, sheriff ↔ evidence); any exit that leads to an exterior room (`out`/`leave` to
   main_street, and so on) returns to the 3D view at that building's door, facing away from it. There is no
   win/lose logic yet; story phases add it.
2. **Items outdoors.** No item is placed outdoors in this phase. Items dropped outdoors go on the zone room's
   floor and are listed by `look`; they get no sprite yet.
3. **Visual target.** `Docs/Design/prototypes/fp_spike.py` and the three frames beside it are the approved
   look (Rowan approved them in chat). Port its sky, mountains, floor, textures, horse and riding overlay.
4. **Where world files are found.** Relative to the working directory: `data/world/` (same as `data/art/`),
   because `./playGame` runs from the repo root on WSL and the lab machines. A missing folder prints one
   warning on stderr naming the path it tried.
5. **Built-in default.** The full town ships as files in `data/world/`. The built-in fallback is a minimal
   single street (enough to start and walk), used only if files are missing, with the stderr warning.
6. **Clock costs.** A step costs 2 minutes on foot or riding (riding covers twice the distance). Turning,
   `help`, `look`, `inventory`, `quit` and unrecognised input are free. Other successful typed commands cost
   5 minutes, `wait` 60, in the 3D view and inside location screens alike.
7. **Terminal and platform.** Linux and WSL only (lab machines over SSH, VS Code terminal, Windows Terminal).
   Minimum 80 × 24; smaller shows "Make the terminal bigger (80 x 24 or more)." until resized.

## Left to the builder
Exact map layout within the README arrangement, prop placement, procedural texture details, colour values,
help wording, the riding overlay art.
