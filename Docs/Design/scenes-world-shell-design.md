# Design: grey-pixel scenes + 18-location world shell

Date: 2026-11-26 · Status: draft for Rowan's approval

## Intent
Replace the top-down box map with the classic text-adventure layout: each of the 18 locations shows one
full-width picture, and the player moves between locations with commands. Pictures are greyscale images
drawn as grey terminal pixels (256-colour background) with a plain-character fallback. Until Rowan's
drawings exist, each location shows a generated placeholder scene. This phase also builds the playable
skeleton the course's "basic implementation" milestone checks: move between rooms, interact with the
environment, and pick up / drop items.

## Goals
- Scene pipeline: greyscale PGM image → terminal picture, ANSI grey mode and plain-character mode.
- Story-tied texture: day 0–1 clean; from day 2 survey digits creep in; from day 5 pixels erase to flat white
  ("past the line is flat white"). Deterministic per pixel so tests are stable. Day stays fixed in this phase.
- Placeholder scene per location when `data/art/<room_id>.pgm` is missing: sky/ground gradient, a building
  or landscape silhouette, and the location name.
- World of 18 rooms using the design report's Room hierarchy (abstract Room; StandardRoom, DarkRoom,
  LockedRoom, HazardRoom, LoopRoom), World, WorldBuilder.
- Command hierarchy from the design report: CommandParser, CommandRegistry, GoCommand, LookCommand,
  TakeCommand, DropCommand, InventoryCommand, HelpCommand, QuitCommand. A Game class runs the loop through
  IDisplay/IInput so it is unit-tested (fixes the old "main.cpp logic untested" finding, and quit is
  normalized).
- Inventory + Item with one neutral object (a tin cup in the Saloon).

## Non-goals (later phases)
NPCs, horse, talk/give/fight/sleep, puzzle items and their effects, day advancing, sanity/health, random
events, endings, the active line.

## Anti-goals
- No art decides gameplay. No Claude attribution. Nothing outside include/src/data/test in game code.
- Not breaking the plain terminal: `--plain` (or no colour support) must still be playable.

## Approaches considered
1. **Greyscale image files rendered by the game (chosen).** Art is data (PGM in `data/art/`), the game
   owns shading, texture and erosion, so the day effects need no redrawing. PGM needs no libraries.
2. Pre-rendered ANSI text files. Simplest to print, but erosion/digits would need parsing escape codes, and
   every day-state would have to be pre-baked.
3. Keep top-down map and add pictures (option B). Rejected by Rowan.

## Picture format (for Rowan's drawings)
- Greyscale, **160 × 40 pixels**, 1 pixel = 1 character cell. Drawing at 320 × 80 is fine; the loader
  downsamples by 2 when the image is exactly double size.
- Save as **PGM** (GIMP: File › Export As › `name.pgm`, choose "ASCII"). Both plain (P2) and binary (P5) load.
- File name = room id: `main_street.pgm`, `saloon.pgm`, … (list below).
- White = brightest, black = darkest.

## World layout (my call, correct me)
Exits are named; directions are just names, so `go north`, `n`, `enter saloon`, `go saloon` all work, and
`leave`/`out` goes back.

| Room id | Type | Exits |
|---|---|---|
| main_street | Standard | east→desert_road, north→old_mine (hill trail), west→dry_well, south→graveyard; enter: hotel_lobby, saloon, gunsmith, sheriffs_office, stable, general_store, church, telegraph_office |
| desert_road | Loop | west→main_street (riding on east is the later hidden ending; blocked with a message for now) |
| graveyard | Hazard | north→main_street, south→soda_lake |
| soda_lake | Hazard | north→graveyard, east→the_line |
| the_line | Hazard | west→soda_lake |
| hotel_lobby | Standard | out→main_street, up→hotel_room, down→hotel_cellar |
| hotel_room | Standard | down→hotel_lobby |
| hotel_cellar | Dark | up→hotel_lobby |
| saloon, gunsmith, stable, general_store, church, telegraph_office | Standard | out→main_street |
| sheriffs_office | Standard | out→main_street, north→evidence_room |
| evidence_room | Locked | south→sheriffs_office |
| old_mine | Dark | south→main_street |
| dry_well | Dark | east→main_street |

Room-type behaviour this phase: DarkRoom shows a near-black picture and "too dark to see" (items hidden);
LockedRoom blocks entry with "locked, you need a key" (mechanism + tests now; the Evidence Room is built
**unlocked** until the iron-key puzzle phase so every room is reachable); HazardRoom and LoopRoom get their
own descriptions now, their effects come with sanity/story.

## Screen layout
Picture (160×40) on top, then room name and description, exits, visible items, the last message, and the
`> ` prompt on the same line. Clears between turns. Fullscreen terminal assumed; README will say so.

## Retired
SceneMap, TownLayout, Building, Entity, Viewport, Renderer and their tests; the top-down main.cpp loop.
Canvas, AsciiArtLibrary-style loading, ConsoleDisplay/ConsoleInput and IDisplay/IInput carry over
(AsciiArtLibrary is replaced by a SceneLibrary that loads PGM).

## Left to the builder
Placeholder art details, exact description text, Bayer/hash choice for texture, help text wording.
