# Renderer Foundation + Main Street Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** A layered ASCII renderer and a walkable Main Street blockout (nine labelled boxes with doors), in the course repo layout.

**Architecture:** Map data (`SceneMap`: size, building footprints, doors) is separate from drawing (`Canvas`, `AsciiArtLibrary`). `Renderer` composes ground → buildings → entities → status into a `Canvas` cropped by a `Viewport` and sends it to `IDisplay`. `main.cpp` is the only interactive code.

**Tech Stack:** C++11, g++ in WSL Ubuntu, GoogleTest, valgrind, lcov, cppcheck, `Makefile.local`.

**Spec:** `Docs/Design/renderer-main-street-design.md`

## Global Constraints

- C++11 only (no `std::make_unique`, no C++14+ features).
- Headers `include/<Class>.hpp` with include guards `<CLASS>_HPP_INCLUDED` and Doxygen `/** */` doc on every class/method; sources `src/<Class>.cpp`; `src/project/main.cpp`; tests `test/test<Class>.cpp` plus `test/main.cpp` (gtest main).
- Only touch: `include/`, `src/`, `src/project/`, `test/`, `data/`, `.gitignore`, `README.md`, plus gitignored `Makefile.local`.
- All build/test commands run in WSL: `wsl -d Ubuntu -- make -f Makefile.local <target>` from the repo root.
- Art never decides collision. Out-of-range input never crashes.
- Commits: no Claude attribution / Co-Authored-By lines. Only commit when Rowan says so.

## Review Focus

1. Player tries to step off the map edge or into a wall → stays put, message "You can't go that way." (Task 5 test `MoveBlockedAtEdge`, Task 4 `WalkableEdges`).
2. Map smaller than the viewport → viewport origin stays at (0,0), never negative (Task 3 `FollowSmallMap`).
3. Text/box drawn partly off-canvas → clipped silently, no crash (Task 1 `ClipsOutOfBounds`).
4. Art file wider/taller than the building footprint, or missing → clipped to footprint / box fallback (Task 4 `ArtClippedToFootprint`, `FallbackBox`).
5. Unknown/empty/mixed-case input ("  N ", "", "dance") → normalised or "Unknown command" without crashing (Task 5 `ParsesMoveInput`).

---

### Task 0: Skeleton, local build, gtest main

**Files:**
- Create: `Makefile.local` (gitignored), `test/main.cpp`, `src/project/main.cpp` (prints nothing yet, returns 0), `data/art/.gitkeep`

`Makefile.local` targets (mirroring the course): `playGame` (src/*.cpp + src/project/main.cpp), `testGame` (src/*.cpp + test/*.cpp, `-lgtest -lpthread`), `memcheck` (`valgrind --leak-check=full --error-exitcode=1 ./testGame`), `coverage` (build tests with `--coverage`, run, `lcov` capture filtered to `src/` excluding `src/project`, print summary), `static` (`cppcheck --enable=all --suppress=missingIncludeSystem include src`), `clean`. Flags: `-std=c++11 -Wall -Wextra -g -Iinclude`.

- [ ] Step 1: Create files. `test/main.cpp` = `InitGoogleTest` + `RUN_ALL_TESTS`.
- [ ] Step 2: Run `make -f Makefile.local playGame testGame` → both build; `./testGame` prints "0 tests".

### Task 1: Geometry + Canvas

**Files:** Create `include/Geometry.hpp` (header-only `struct Point {int x; int y;}` with `==`, `struct Rect {int x, y, width, height; bool contains(Point) const;}`), `include/Canvas.hpp`, `src/Canvas.cpp`, `test/testCanvas.cpp`, `test/testGeometry.cpp`

**Interfaces — Produces:**
- `Canvas(int width, int height, char fill = ' ')`
- `int width() const; int height() const;`
- `void put(int x, int y, char c);` (ignored out of bounds)
- `char get(int x, int y) const;` (returns `' '` out of bounds)
- `void drawText(int x, int y, const std::string& text);` (clipped)
- `void drawBox(const Rect& r);` (`+` corners, `-` top/bottom, `|` sides; interior untouched)
- `void blit(int x, int y, const std::string& block);` (multi-line text, clipped; spaces are transparent)
- `std::string toString() const;` (rows joined by `\n`, trailing `\n`)

- [ ] Step 1: Tests: `RectContains` (Rect{2,2,3,2}: (2,2) yes, (4,3) yes, (5,3) no); `PutGet`; `ClipsOutOfBounds` (put(-1,0), drawText(8,0,"hello") on 10×1 → `"        he\n"`); `DrawBox` (3×3 box on 3×3 canvas → `"+-+\n| |\n+-+\n"`); `BlitTransparent` (blit "a b" over "xxx" → "axb").
- [ ] Step 2: Run `./testGame` → fails to compile/link.
- [ ] Step 3: Implement.
- [ ] Step 4: Run `./testGame` → all pass.

### Task 2: AsciiArtLibrary

**Files:** Create `include/AsciiArtLibrary.hpp`, `src/AsciiArtLibrary.cpp`, `test/testAsciiArtLibrary.cpp`, `test/data/art/hotel.txt` (any 2-line sample)

**Interfaces — Produces:**
- `bool loadFromDirectory(const std::string& path);` loads every `*.txt`, key = filename without extension; false if folder missing. Use POSIX `opendir/readdir` (C++11 has no `<filesystem>`).
- `void addArt(const std::string& key, const std::string& art);`
- `std::string getArt(const std::string& key) const;` ("" if absent)
- `bool hasArt(const std::string& key) const;`

- [ ] Step 1: Tests: `LoadsDirectory` (`test/data/art` → hasArt("hotel"), getArt matches file); `MissingDirectory` → false; `UnknownKey` → "" / false; `AddArt`.
- [ ] Step 2: Run → fail. Step 3: Implement. Step 4: Run → pass.

### Task 3: Viewport

**Files:** Create `include/Viewport.hpp`, `src/Viewport.cpp`, `test/testViewport.cpp`

**Interfaces — Produces:** `Viewport(int width, int height)`, `void follow(Point target, int mapWidth, int mapHeight)`, `Point origin() const`, `int width() const`, `int height() const`, `Point toScreen(Point world) const`.

Algorithm: `ox = clamp(target.x - width/2, 0, max(0, mapWidth - width))`, same for y.

- [ ] Step 1: Tests: `FollowCentres` (vp 10×4, map 100×20, target (50,10) → origin (45,8)); `FollowClampsLow` (target (0,0) → (0,0)); `FollowClampsHigh` (target (99,19) → (90,16)); `FollowSmallMap` (map 5×2 → (0,0)); `ToScreen`.
- [ ] Step 2–4: fail → implement → pass.

### Task 4: Building, SceneMap, TownLayout, Renderer

**Files:** Create `include/Building.hpp` (struct: `std::string id, label; Rect footprint; Point door;`), `include/SceneMap.hpp`, `src/SceneMap.cpp`, `include/TownLayout.hpp`, `src/TownLayout.cpp`, `include/Entity.hpp` (struct `std::string id; Point pos; char glyph;`), `include/Renderer.hpp`, `src/Renderer.cpp`, tests `test/testSceneMap.cpp`, `test/testTownLayout.cpp`, `test/testRenderer.cpp`

**Interfaces — Produces:**
- `SceneMap(const std::string& name, int width, int height, char ground = ' ')`; `name()`, `width()`, `height()`, `ground()`; `void addBuilding(const Building&)`; `const std::vector<Building>& buildings() const`; `bool isInside(Point) const`; `bool isWalkable(Point) const` (inside and not in any footprint unless it is that building's door); `const Building* buildingAtDoor(Point) const` (nullptr if none); `void addRoad(int y)` / `bool isRoad(int y) const` for drawing `=`.
- `SceneMap TownLayout::buildMainStreet();` — map 90×13; north row y=1..3 boxes HOTEL, SALOON, GUNSMITH, SHERIFF, STABLE; road `=` on y=6; south row y=9..11 boxes CHURCH, GENERAL STORE, TELEGRAPH, GRAVES; doors on north-row bottom edge / south-row top edge (facing the road); ids lowercase (`hotel`, `saloon`, `gunsmith`, `sheriff`, `stable`, `church`, `general_store`, `telegraph`, `graves`). `Point TownLayout::startPosition()` = (88,6) (east edge on the road).
- `Renderer(const AsciiArtLibrary& art, Viewport& viewport)`; `std::string renderFrame(const SceneMap& map, const std::vector<Entity>& entities, Point focus, const std::string& status)` — follows focus, draws ground, road, each building (art via `blit` clipped to footprint if `hasArt(id)`, else `drawBox` + centred label on middle row), door as `D`, entities, then a status line below the view; returns `Canvas::toString()` of viewport-sized canvas + status row.

- [ ] Step 1: Tests:
  - SceneMap: `WalkableEdges` (outside map false, (0,0) true); `FootprintBlocks`; `DoorWalkable`; `BuildingAtDoor`.
  - TownLayout: `HasNineBuildings`; `NoOverlaps` (pairwise footprints disjoint); `DoorsOnEdgeAndReachable` (each door on footprint edge, cell toward road walkable); `StartWalkable`.
  - Renderer: `DrawsPlayerAtFocus` (small map, entity '@' appears at screen pos); `FallbackBox` (box chars + label present); `ArtClippedToFootprint` (art 'XXXXXXXX' wider than footprint does not leak past it); `StatusLineLast`.
- [ ] Step 2–4: fail → implement → pass.

### Task 5: Display/Input interfaces, movement, main

**Files:** Create `include/IDisplay.hpp`, `include/IInput.hpp` (pure virtual, virtual dtor), `include/ConsoleDisplay.hpp`, `src/ConsoleDisplay.cpp`, `include/ConsoleInput.hpp`, `src/ConsoleInput.cpp`, `include/Movement.hpp`, `src/Movement.cpp`, `test/testMovement.cpp`, `test/testConsoleDisplay.cpp`; Modify `src/project/main.cpp`

**Interfaces — Produces:**
- `IDisplay`: `show(const std::string&)`, `showArt(const std::string&)`, `clear()`. `ConsoleDisplay(std::ostream& out = std::cout)`; `clear()` writes `"\033[2J\033[H"`.
- `IInput`: `std::string readLine()`. `ConsoleInput(std::istream& in = std::cin)`; returns `"quit"` at EOF.
- `Movement`: `enum class Direction { North, South, East, West, None };` `Direction parseDirection(const std::string& word)` (trim + lowercase; `n/north/s/south/e/east/w/west`); `Point step(Point p, Direction d)`; `bool tryMove(const SceneMap& map, Point& pos, Direction d)` (moves only if walkable).
- main loop: render → read → `quit` exits; direction moves or prints "You can't go that way."; else "Unknown command. Try n, s, e, w or quit." Standing on a door shows "<LABEL> (closed for now)".

- [ ] Step 1: Tests: `ParsesMoveInput` ("  N " → North, "west" → West, "" / "dance" → None); `StepDirections`; `MoveBlockedAtEdge`; `MoveBlockedByWall`; `MoveOk`; `ConsoleDisplayWrites` (ostringstream gets text/art/clear code); `ConsoleInputEof` (empty istringstream → "quit").
- [ ] Step 2–4: fail → implement → pass.
- [ ] Step 5: `make -f Makefile.local playGame` and pipe `printf 'w\nw\nn\nquit\n' | ./playGame` → frames redraw, no crash.

### Task 6: Verify + README

- [ ] `make -f Makefile.local memcheck` → "0 errors", no leaks.
- [ ] `make -f Makefile.local coverage` → lines >90% for `src/` excluding `src/project`.
- [ ] `make -f Makefile.local static` → no errors.
- [ ] Update README "Project structure" only if layout differs.
- [ ] Ask Rowan before committing (branch `feature/renderer-main-street`, no attribution).
