# First-Person Colour 3D Exterior Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Walkable first-person colour 3D town exterior with typed interaction, horse, day/night and editable world files.

**Architecture:** Pure, tested core (FrameBuffer, TerminalPainter, KeyDecoder, WorldMap + parsers, Camera, Raycaster, GameClock/SkyModel, ExteriorGame) driven by thin untested terminal I/O (RawTerminal, TerminalKeyInput, main). Shares Player/GameState/World with the existing location game.

**Tech Stack:** C++11, POSIX termios/ioctl, GoogleTest, WSL `make -f Makefile.local`.

**Spec:** `Docs/Design/exterior-3d-design.md` (visual target: `Docs/Design/prototypes/`)

## Global Constraints

- C++11 game code; headers `include/*.hpp`; tests `test/test*.cpp`; data under `data/world/`. No new libraries.
- Half-block `▀` (UTF-8 `\xE2\x96\x80`), fg = top pixel `\033[38;2;R;G;Bm`, bg = bottom pixel `\033[48;2;R;G;Bm`.
- Text area: 3 rows (status, message, input). View pixels: cols × (rows − 3) × 2. Minimum terminal 80 × 24.
- Clock: start 17:30; step 2 min; successful typed command 5; `wait` 60; turn/help/look/inventory/quit/unknown 0.
- Riding: step distance × 2, eye height 0.72 (on foot 0.5). Enter range 1.5 tiles. Mount range 1.5 tiles.
- Missing world files → stderr warning naming the path, built-in minimal street.
- No Claude attribution in commits.

## Review Focus

1. Terminal left in raw mode after a crash/Ctrl-C/EOF → must restore (Task 2 `RawTerminal` restore path; manual check in Task 6).
2. Escape sequence split across reads (`ESC` then `[A`) → still one Up key (Task 2 `SplitEscape`).
3. Malformed world files (bad numbers, unknown letters, ragged rows) → line skipped / tile treated as sand, no crash (Task 3 `Malformed*`).
4. Walking into corners / diagonal into walls → slides, never ends inside a wall (Task 4 `SlideAlongWall`, `NeverInsideWall`).
5. Terminal resized smaller than 80 × 24 → message instead of a broken frame (Task 6 `TooSmallTerminal`).

---

### Task 1: Rgb, FrameBuffer, TerminalPainter
**Files:** `include/Rgb.hpp`, `include/FrameBuffer.hpp`, `src/FrameBuffer.cpp`, `include/TerminalPainter.hpp`, `src/TerminalPainter.cpp`, tests `test/testFrameBuffer.cpp`, `test/testTerminalPainter.cpp`
**Produces:** `struct Rgb {unsigned char r,g,b; ...}` with `==`, `Rgb mix(Rgb,Rgb,double)`, `Rgb scale(Rgb,double)`; `FrameBuffer(int w,int h)`, `width/height/get/set/fill` (clipped); `TerminalPainter`: `std::string paintFull(const FrameBuffer&, int topRow)` (cursor home at topRow, all cells), `std::string paintDiff(const FrameBuffer& prev, const FrameBuffer& next, int topRow)` (only changed cells, each run starts with `\033[row;colH`), `std::string paintPlain(const FrameBuffer&)` (luminance ramp, no escapes); odd heights pad with black.
- [ ] Tests: `SetGetClip`, `MixScale`, `FullUsesHalfBlocks` (2×2 → 2 `▀`, colours of top/bottom), `DiffEmptyWhenSame`, `DiffOnlyChanged` (one pixel changed → one cell, one cursor move), `PlainNoEscapes`, `OddHeightPads`.
- [ ] Fail → implement → pass → commit.

### Task 2: KeyDecoder, RawTerminal, TerminalKeyInput
**Files:** `include/Key.hpp`, `include/KeyDecoder.hpp`, `src/KeyDecoder.cpp`, `include/IKeyInput.hpp`, `include/RawTerminal.hpp`, `src/RawTerminal.cpp`, `include/TerminalKeyInput.hpp`, `src/TerminalKeyInput.cpp`, test `test/testKeyDecoder.cpp`
**Produces:** `enum class KeyType {Up,Down,Left,Right,Char,Backspace,Enter,Eof,None}; struct Key {KeyType type; char ch;}`; `KeyDecoder::feed(char)` → `std::vector<Key>` ready keys (state machine; ESC alone buffered), `flush()` → pending ESC as None; `IKeyInput::readKey()`; `RawTerminal` RAII (termios raw, alt screen `\033[?1049h`, hide cursor, restore in dtor + atexit + SIGINT/SIGTERM handler), `static bool size(int& cols,int& rows)`, `static bool isInteractive()` (isatty stdin && stdout).
- [ ] Tests: arrows, `SplitEscape`, printable chars, Backspace (127 and 8), Enter (`\r` and `\n`), Ctrl-D → Eof, unknown sequence `ESC [ Z` → None.
- [ ] Fail → implement → pass → commit.

### Task 3: World data + files
**Files:** `include/WorldMap.hpp`, `src/WorldMap.cpp`, `include/WorldFiles.hpp`, `src/WorldFiles.cpp`, data `data/world/{town.map,buildings.txt,zones.txt}`, tests `test/testWorldMap.cpp`, `test/testWorldFiles.cpp`
**Produces:** `enum class Ground {Sand,Street,Boardwalk,Grass,Salt}`; `struct BuildingDef {char letter; std::string id,label,style,roomId; double height; Rgb base,trim; int doorX,doorY; char facing;}`; `struct Zone {std::string roomId; Rect area; std::string description;}`; `WorldMap`: `width/height`, `char tile(x,y)` ('#' outside), `bool isBlocked(double x,double y)`, `Ground groundAt(double,double)`, `const BuildingDef* buildingAt(int,int)`, `const Zone* zoneAt(double,double)`, `const BuildingDef* doorNear(double x,double y,double range)`, `Point2 start()` (`struct Point2{double x,y;}`), `void addBuilding/addZone/setTiles`. Parsers: `bool parseTownMap(const std::string&, WorldMap&)`, `int parseBuildings(const std::string&, WorldMap&, std::ostream& warn)` (returns count), `int parseZones(...)`; `WorldMap loadWorld(const std::string& dir, std::ostream& warn)`; `WorldMap defaultWorld()`.
Formats — buildings: `a hotel "HOTEL" 1.9 planks 150,88,62 235,220,190 4,4 S hotel_lobby`; zones: `main_street 1 4 40 8 Main Street runs east to west...`; `#` comments and blank lines ignored.
Town (~64×40): README rows along a street at y≈20; desert road east to x=63; graves fence + gate south of the street → graveyard → salt lake → The Line at the far south; hill rock `#` north with a trail and mine door; well `O` at the west end.
- [ ] Tests: map parse + ragged rows padded, start `@`, unknown char → sand; buildings good line, `MalformedBuildingSkipped` (warning written), zones; `isBlocked` for each blocker, door tile passable; `zoneAt`; `doorNear` range; `LoadsShippedWorld` (data/world loads: 9 buildings + mine + well doors, 5 zones, start walkable); `MissingDirFallsBack`.
- [ ] Fail → implement → pass → commit.

### Task 4: Camera + collision
**Files:** `include/Camera.hpp`, `src/Camera.cpp`, `test/testCamera.cpp`
**Produces:** `Camera(double x,double y,double angle)`; `x/y/angle/dirX/dirY/planeX/planeY`; `turn(double radians)`; `bool step(const WorldMap&, double dist)` (positive forward, negative back; axis-separated sliding with 0.2 radius; returns moved); constants `kTurnStep = 0.2618` (15°), `kWalkStep = 0.5`.
- [ ] Tests: `TurnWraps`, `StepForward`, `BlockedByWall`, `SlideAlongWall` (diagonal into a wall moves along it), `NeverInsideWall` (100 random steps).
- [ ] Fail → implement → pass → commit.

### Task 5: Raycaster (day only)
**Files:** `include/Raycaster.hpp`, `src/Raycaster.cpp`, `include/WallTextures.hpp`, `src/WallTextures.cpp`, `test/testRaycaster.cpp`
**Produces:** `struct SkyState {Rgb top, horizon; double light; double sunAngle; bool moon; double stars;}` (default = sunset); `struct RenderInput {const WorldMap* map; Camera cam; double eyeHeight; SkyState sky; bool riding;}`; `void renderView(const RenderInput&, FrameBuffer&, std::vector<double>& depth)`; `Rgb wallTexel(const BuildingDef&, double u, double v, bool facade, bool night)`. Port the prototype's sky gradient, sun, mountain ridge, floor casting by `Ground`, DDA walls with heights, fog to horizon colour, rock/fence/well styles.
- [ ] Tests: `SkyAboveHorizon` (top row ≈ sky top), `WallAhead` (facing a building: centre column has texture colour, depth set), `OpenDesertNoWall`, `FogFarther` (far wall closer to horizon colour than near), `DeterministicFrame`.
- [ ] Fail → implement → pass → commit.

### Task 6: ExteriorGame + main — FIRST PLAYABLE (show Rowan)
**Files:** `include/ExteriorGame.hpp`, `src/ExteriorGame.cpp`, `test/testExteriorGame.cpp` (fake keys + fake screen sink); Modify `src/project/main.cpp`
**Produces:** `class IScreen { virtual void write(const std::string&)=0; virtual bool size(int&,int&)=0; }`; `ExteriorGame(WorldMap&, World&, Player&, GameState&, CommandRegistry&, IKeyInput&, IScreen&, bool plain)`; `void run()`; `bool handleKey(const Key&)`; `std::string runCommand(const std::string&)`; `const std::string& inputLine()`. Arrows move/turn and set current room from zone; chars build the input line; Enter runs: exterior verbs first (`look`, `help`, `quit`, `wait`), else registry (take/drop/inventory). Status row: `ZONE NAME   17:30   on foot`. Frame = full paint first, diff after; too small → message.
main: if `RawTerminal::isInteractive()` → exterior game, else existing location `Game`.
- [ ] Tests: arrows move camera, typing + Enter runs `look` (zone description), unknown command message, `quit` ends, `Eof` ends, `TooSmallTerminal`, zone change updates player room, frames after the first are diffs.
- [ ] Fail → implement → pass → commit → manual run → show Rowan.

### Task 7: Sprites, props, horse
**Files:** `include/Sprite.hpp`, `src/Sprite.cpp`, `include/Horse.hpp`, `src/Horse.cpp`; extend WorldFiles (`parseSprite`, `parseProps`), Raycaster (billboards with depth, riding overlay), ExteriorGame (`mount`/`ride horse`, `dismount`, speed, eye height); data `data/world/props.txt`, `data/world/sprites/*.txt` (horse, headstone, cross, cactus, barrel, trough, hitching_post, wagon, mine_lamp); tests `test/testSprite.cpp`, `test/testHorse.cpp`, additions to raycaster/exterior tests.
- [ ] Tests: sprite parse (palette, transparency, size line, malformed), props parse, `SpriteHiddenBehindWall`, `SpriteVisible`, mount out of range refused, mount/dismount state + horse position, riding doubles step, riding overlay pixels present at bottom, blocking props block movement, can't enter door mounted (Task 9 test).
- [ ] Fail → implement → pass → commit.

### Task 8: Clock, sky, night
**Files:** `include/GameClock.hpp`, `src/GameClock.cpp`, `include/SkyModel.hpp`, `src/SkyModel.cpp`; data `data/world/sky.txt`; extend Raycaster (stars, moon, lit windows at night, light scaling), ExteriorGame (costs, status time), tests `test/testGameClock.cpp`, `test/testSkyModel.cpp`.
- [ ] Tests: clock advance/wrap/format "17:30"/day rollover count, costs per action, sky keyframe interpolation (midpoint), parse malformed, `NightHasStars`, `WindowsLitAtNight`, darker light at midnight.
- [ ] Fail → implement → pass → commit.

### Task 9: Doors
**Files:** extend ExteriorGame; reuse `Game` screens via a `LocationSession` helper (runs typed commands on interior rooms until an exit returns to an exterior room), tests in `test/testExteriorGame.cpp`.
- [ ] Tests: `enter saloon` near door opens saloon screen; `enter` too far → "There's no door here."; mounted → "Dismount first."; locked room refused; `leave` returns outside the door facing away; interior moves (lobby → up → hotel room) work.
- [ ] Fail → implement → pass → commit.

### Task 10: Verify + world-files guide
- [ ] memcheck 0 errors, coverage > 90%, cppcheck clean, render-time measurement (160×80 frame ≤ 30 ms) reported honestly.
- [ ] Manual play in WSL: colour frame, movement, horse, night, doors, Ctrl-C restores terminal.
- [ ] Write `data/world/README.md` explaining every file format with examples.
- [ ] Final whole-branch review; fix Critical/Important test-first.
