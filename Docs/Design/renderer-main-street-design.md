# Design: ASCII renderer foundation + Main Street blockout

Date: 2026-11-26 · Status: approved by Rowan

No ASCII art exists yet: every building renders as a plain labelled box. `AsciiArtLibrary` stays as the seam
where real art drops in later without touching collision or game logic.

## Intent
Build Phase B (renderer foundation) of `the-line-playable-town-build-plan.md` and the first slice of Phase C
(Main Street as labelled boxes), inside this personal GitHub repo, laid out exactly like the course GitLab repo
so the files can be copied over unchanged once the group repo is available.

## Goals
- Course-compatible layout: `include/*.hpp`, `src/*.cpp`, `src/project/main.cpp`, `test/*.cpp` (GoogleTest),
  `data/art/`.
- Layered character-grid renderer with a viewport that follows the player.
- Main Street with the nine README buildings as labelled boxes with doors; collision from footprints only.
- `main.cpp` demo: draw Main Street, accept `n/s/e/w` and `quit`, show collision + viewport scroll.
- Unit tests for every non-interactive class (course wants >90% coverage).

## Non-goals
Entering buildings, other rooms, NPCs, horse, inventory, commands framework, story — later phases.

## Anti-goals
- No root `Makefile` in git (course supplies it; not on the allowed-file list).
- No Claude attribution in commits.
- Art must never drive collision.

## Constraints
- C++11 only (course Makefile standard unknown).
- Build/test locally in WSL Ubuntu (g++ 15, gtest, valgrind, lcov, cppcheck).
- Local build via `Makefile.local` (gitignored), targets `playGame`, `testGame`, `memcheck`, `coverage`.

## Approaches considered
1. **Layered cell grid (chosen):** map data (size, walkability, building footprints + doors) separate from glyphs;
   renderer composes ground → buildings → NPCs → player → status into a frame. Matches the build plan's art contract.
2. Whole-room art files with player glyph overlaid: simpler, but collision would have to come from the art,
   which the build plan forbids, and the art teammate couldn't replace art freely.

## Components
- `Canvas` — W×H char grid: `put`, `get`, `drawText`, `drawBox`, `toString`. Out-of-bounds writes ignored.
- `Point`/`Rect` — small value types.
- `Building` — id, label, footprint `Rect`, door `Point` (door sits on the footprint's bottom edge).
- `SceneMap` — name, size, buildings; `isInside`, `isWalkable` (inside map and not in a footprint, except the door
  cell), `buildingAtDoor`.
- `Viewport` — width/height, `follow(target, mapW, mapH)` centres and clamps; `origin()`.
- `AsciiArtLibrary` — `loadFromDirectory`, `getArt`, `hasArt` (design report API); renderer uses art when present,
  else a `+--+` box with the label.
- `IDisplay` / `ConsoleDisplay` (`show`, `showArt`, `clear` via ANSI clear code) and `IInput` / `ConsoleInput`
  (`readLine`, returns "quit" at EOF).
- `Renderer` — composes a frame from SceneMap + art + entity glyphs + status line, cropped to the viewport.
- `TownLayout` — builds the Main Street `SceneMap` (code-defined for now; data file later if the art teammate needs it).

## Left to the builder
Exact coordinates, viewport size (≈80×20), status line wording.
