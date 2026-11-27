# Grey-Pixel Scenes + 18-Location World Shell Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Location-to-location play across 18 rooms, each shown as a greyscale picture (ANSI grey or plain characters, with day-tied survey digits/erasure), with go/look/take/drop/inventory/help/quit.

**Architecture:** Art is greyscale PGM data (`GreyImage`), rendered by `SceneRenderer`; missing art becomes a generated placeholder (`SceneLibrary`). The design report's Room hierarchy, World/WorldBuilder, Command/CommandRegistry/CommandParser and a testable `Game` loop run the play.

**Tech Stack:** C++11 game code, GoogleTest (C++17 test build), WSL `make -f Makefile.local`.

**Spec:** `Docs/Design/scenes-world-shell-design.md`

## Global Constraints

- C++11 only in include/ and src/ (`std::unique_ptr` yes, `std::make_unique` no). Lines ≤ 80 columns.
- Headers `include/<Class>.hpp`, guard `<CLASS>_HPP_INCLUDED`, Doxygen on every class/method; tests `test/test<Thing>.cpp`.
- Picture size 160 × 40; images exactly 320 × 80 are downsampled by 2. Ids = room ids from the spec table.
- Digits: day ≤ 1 none; day d ≥ 2 → `min(60, (d - 1) * 8)` % of cells. Erase to white (255): day < 5 none; 5 → 10%, 6 → 25%, 7 → 45%, > 7 → 70%.
- Grey code: background `\033[48;5;Nm`, N = 232 + round(v × 23 / 255). Rows end with `\033[0m`.
- Plain ramp `" .:-=+*#%@"` (dense = bright), 4×4 Bayer dither.
- Commands run via `make -f Makefile.local run-tests` in WSL. No Claude attribution in commits.

## Review Focus

1. Malformed, truncated or wrong-size PGM in data/art → no crash; placeholder or as-is image (Task 1 `RejectsMalformed`, `RejectsTruncated`; Task 3 `BadFileFallsBack`).
2. Odd spacing/case ("  ENTER   Saloon ", "Quit") → understood (Task 6 `ParserNormalizes`; Task 7 `QuitAnyCase`).
3. Unknown or partial exit ("go sal", "go") → friendly message, no move (Task 6 `GoUnknownExit`, `GoNothing`).
4. Taking things in an unlit dark room → hidden (Task 6 `TakeInDarkRoom`).
5. Terminal without colour → `--plain` output contains no escape codes (Task 2 `PlainHasNoEscapes`).

---

### Task 1: GreyImage + PGM parsing

**Files:** Create `include/GreyImage.hpp`, `src/GreyImage.cpp`, `include/Pgm.hpp`, `src/Pgm.cpp`, `test/testGreyImage.cpp`, `test/testPgm.cpp`, `test/data/art/tiny.pgm` (P2, 2×2, values 0 85 170 255)

**Produces:**
- `GreyImage(int width, int height, int fill = 0)`; `int width() const`; `int height() const`; `int get(int x, int y) const` (0 off-image); `void set(int x, int y, int v)` (clamped 0..255, ignored off-image); `void fillRect(const Rect& r, int v)`; `GreyImage halved() const` (2×2 average).
- `bool parsePgm(const std::string& data, GreyImage& out)` (P2 and P5, `#` comments, maxval scaled to 255; false on bad magic, non-positive size, missing/short data); `bool loadPgm(const std::string& path, GreyImage& out)`.

- [ ] Tests: `GetSetClamp` (set 300 → 255, set -4 → 0, off-image get 0); `FillRect`; `HalvedAverages` (2×2 of 0,100,200,100 → 1×1 of 100); `ParsesP2WithComment`; `ScalesMaxval` (maxval 15, value 15 → 255); `ParsesP5`; `RejectsMalformed` ("P7", "P2 0 4 255"); `RejectsTruncated` (P2 2×2 with 3 values); `LoadsFile` (tiny.pgm); `MissingFile`.
- [ ] Run → fails to compile. Implement. Run → pass. Commit.

### Task 2: SceneRenderer

**Files:** Create `include/SceneRenderer.hpp`, `src/SceneRenderer.cpp`, `test/testSceneRenderer.cpp`

**Consumes:** GreyImage. **Produces:** `enum class ColourMode { Grey, Plain };` `int digitPercent(int day)`; `int erasePercent(int day)`; `std::string renderScene(const GreyImage& img, int day, ColourMode mode)`.

Per cell: `h = (x * 73856093u) ^ (y * 19349663u)`; erased if `h % 100 < erasePercent(day)` (value 255, no glyph); else digit if `(h / 100) % 100 < digitPercent(day)`, digit `'0' + (h / 10000) % 10`. Grey mode prints a space (or digit) on a grey background with foreground N ± 7 (+7 when v < 128) clamped to 232..255. Plain mode uses the dithered ramp; digits replace the ramp char; erased cells print `'@'`.

- [ ] Tests: `DigitAndEraseTables` (day 1 → 0/0, day 2 → 8/0, day 5 → 32/10, day 7 → 48/45, day 9 → 60/70); `PlainKnownRamp` (1×1 of 0 → " ", 255 → "@"); `PlainHasNoEscapes` (no `'\033'` for day 7 output); `GreyCodes` (black pixel contains `48;5;232m`, white `48;5;255m`, each row ends `\033[0m\n`); `RowCount` (height lines); `CleanOnDayOne` (no digits in plain output of an all-128 image); `DigitsCreepIn` (day 7 all-128 80×20 image: digit count between 30% and 66% of non-erased cells); `ErasureOnDayFive` (day 5 all-0 image has some `'@'`, day 4 none); `Deterministic`.
- [ ] Fail → implement → pass → commit.

### Task 3: Placeholders + SceneLibrary

**Files:** Create `include/SceneKind.hpp` (`enum class SceneKind { Outdoor, Indoor, Dark };`), `include/SceneLibrary.hpp`, `src/SceneLibrary.cpp`, `test/testSceneLibrary.cpp`, fixtures `test/data/scenes/big.pgm` (P2 320×80 all 200, generate with a loop in the test setup? no — commit a generated file), `test/data/scenes/broken.pgm` ("P2\n3 3\n255\n1 2")

**Produces:**
- `const int kSceneWidth = 160; const int kSceneHeight = 40;`
- `GreyImage makePlaceholder(SceneKind kind)` — Outdoor: bright sky gradient over darker ground with a central building silhouette; Indoor: mid-grey walls, darker floor, a lighter door rectangle; Dark: every pixel ≤ 20.
- `SceneLibrary`: `bool loadFromDirectory(const std::string& path)` (each `*.pgm`, key = filename stem, 320×80 → halved, unparsable files skipped); `bool has(const std::string& key) const`; `GreyImage sceneFor(const std::string& key, SceneKind kind) const` (loaded image or placeholder).

- [ ] Tests: `PlaceholderSizes` (all kinds 160×40); `DarkIsDark`; `OutdoorSkyBrighterThanGround` (row 2 average > row 38 average); `IndoorNotFlat`; `LoadsAndHalves` (big.pgm → 160×40, value 200); `BadFileFallsBack` (broken not loaded, sceneFor gives placeholder); `MissingDirectory` false; `UnknownKeyPlaceholder`.
- [ ] Fail → implement → pass → commit.

### Task 4: TextUtil, Item, Inventory

**Files:** Create `include/TextUtil.hpp`, `src/TextUtil.cpp`, `include/Item.hpp`, `src/Item.cpp`, `include/Inventory.hpp`, `src/Inventory.cpp`, tests `test/testTextUtil.cpp`, `test/testInventory.cpp`

**Produces:**
- `std::string normalize(const std::string& s)` (trim, lowercase, collapse runs of whitespace to one space); `std::string joinList(const std::vector<std::string>& items)` ("a", "a and b", "a, b and c").
- `class Item { Item(name, description); virtual ~Item(); const std::string& getName() const; const std::string& getDescription() const; virtual std::string examine() const; bool matches(const std::string& word) const; }` — matches when normalized word equals the name or its last word ("cup" matches "tin cup").
- `class Inventory { explicit Inventory(int capacity); std::unique_ptr<Item> add(std::unique_ptr<Item> item); /* nullptr = stored, else handed back */ std::unique_ptr<Item> remove(const std::string& word); const Item* find(const std::string& word) const; bool isFull() const; int size() const; std::vector<std::string> names() const; std::string list() const; }` — list: "You are carrying nothing." / "You are carrying a tin cup." via joinList of names prefixed by nothing extra ("You are carrying tin cup and lantern.").

- [ ] Tests: `NormalizeTrimsCaseSpaces` ("  ENTER   Saloon " → "enter saloon"; "" → ""); `JoinList` (0–3 items); `ItemMatches` ("Cup", "tin cup" yes; "tin" no); `ExamineDefault`; `AddFindRemove`; `AddWhenFullHandsBack` (capacity 1); `RemoveMissingNull`; `ListEmptyAndFull`.
- [ ] Fail → implement → pass → commit.

### Task 5: Room hierarchy, World, WorldBuilder

**Files:** Create `include/Room.hpp`, `src/Room.cpp`, `include/StandardRoom.hpp`, `src/StandardRoom.cpp`, `include/DarkRoom.hpp`, `src/DarkRoom.cpp`, `include/LockedRoom.hpp`, `src/LockedRoom.cpp`, `include/HazardRoom.hpp`, `src/HazardRoom.cpp`, `include/LoopRoom.hpp`, `src/LoopRoom.cpp`, `include/World.hpp`, `src/World.cpp`, `include/WorldBuilder.hpp`, `src/WorldBuilder.cpp`, tests `test/testRooms.cpp`, `test/testWorld.cpp`, `test/testWorldBuilder.cpp`

**Produces (Room):** `Room(const std::string& id, const std::string& name, const std::string& description, SceneKind kind)`; virtual dtor; `getId()`, `getName()`; `virtual SceneKind sceneKind() const`; `void setExit(const std::string& name, Room* to)`; `Room* getExit(const std::string& name) const` (normalized); `std::vector<std::string> exitNames() const` (insertion order); `void addItem(std::unique_ptr<Item>)`; `std::unique_ptr<Item> removeItem(const std::string& word)`; `const Item* findItem(const std::string& word) const`; `virtual bool itemsVisible() const` (true); `virtual std::string describe() const = 0`; `virtual bool canEnter(std::string& why) const` (true); `virtual std::string noExitMessage(const std::string& exit) const` ("You can't go that way."). Protected helpers `std::string itemsLine() const` ("You see a tin cup." or "" ), `std::string exitsLine() const` ("Exits: north, saloon.").
- StandardRoom: description + items + exits.
- `DarkRoom`: `illuminate()`, `isLit()`; unlit: describe "It is too dark to see." + exits, `itemsVisible()` false, `sceneKind()` Dark; lit: like Standard, kind from ctor.
- `LockedRoom(id, name, desc, kind, const std::string& keyId)`: `bool unlock(const std::string& keyId)`, `isLocked()`; `canEnter` false with why "The door is locked. You need a key." while locked.
- `HazardRoom(id, name, desc, kind, const std::string& unease)`: describe adds the unease line.
- `LoopRoom(id, name, desc, kind, const std::string& onwardExit, const std::string& onwardMessage)`: `noExitMessage(onwardExit)` returns onwardMessage.
- `World`: `Room* addRoom(std::unique_ptr<Room>)`; `Room* getRoom(const std::string& id) const`; `void connect(Room* a, const std::string& aExit, Room* b, const std::string& bExit)`; `void setStartRoom(Room*)`; `Room* getStartRoom() const`; `int roomCount() const`; `std::vector<Room*> rooms() const`.
- `WorldBuilder::build()` → `std::unique_ptr<World>`, rooms/types/exits exactly per the spec table, plus Main Street building exit names `hotel, saloon, gunsmith, sheriff, stable, store, church, telegraph` (each building returns with `out`); desert_road onward exit `east` with message "The road east shimmers. You're not ready to leave yet."; evidence_room key `iron_key`, unlocked at build; tin cup ("A dented tin cup. It smells of whiskey.") in saloon; start room desert_road.

- [ ] Tests: rooms — `StandardDescribe` (desc, "You see a tin cup.", "Exits: out."); `ExitLookupNormalized`; `DarkHidesUntilLit`; `LockedBlocksUntilUnlocked` (wrong key false); `HazardAddsUnease`; `LoopOnwardMessage` (and other exits default message). world — `AddGetConnect`, `UnknownRoomNull`, `StartRoom`. builder — `EighteenRooms`; `TypeCounts` (Standard 10, Dark 3, Locked 1, Hazard 3, Loop 1 via dynamic_cast); `AllReachableAndReturnable` (BFS from start reaches all 18; every exit target has some exit back to the source); `TinCupInSaloon`; `EvidenceRoomEnterable`; `StartIsDesertRoad`.
- [ ] Fail → implement → pass → commit.

### Task 6: Player, GameState, parser, registry, commands

**Files:** Create `include/Player.hpp`, `src/Player.cpp`, `include/GameState.hpp`, `src/GameState.cpp`, `include/CommandParser.hpp`, `src/CommandParser.cpp`, `include/Command.hpp`, `include/CommandRegistry.hpp`, `src/CommandRegistry.cpp`, `include/Commands.hpp` (declares the seven commands), `src/GoCommand.cpp`, `src/LookCommand.cpp`, `src/TakeCommand.cpp`, `src/DropCommand.cpp`, `src/InventoryCommand.cpp`, `src/HelpCommand.cpp`, `src/QuitCommand.cpp`, tests `test/testCommandParser.cpp`, `test/testCommands.cpp`

**Produces:**
- `Player(Room* start, int capacity)`: `Room* getCurrentRoom() const`, `void moveTo(Room*)`, `Inventory& getInventory()`.
- `GameState`: `int getDay() const` (starts 1), `void setDay(int)`, `void setFlag(const std::string&)`, `bool hasFlag(...) const`, `void requestQuit()`, `bool quitRequested() const`.
- `struct ParsedCommand { std::string verb, noun, target; bool isEmpty() const; };` `ParsedCommand CommandParser::parse(const std::string& line)` — normalize; verb = first word; noun = rest; if rest contains " to " or " on ", noun = before, target = after.
- `class Command { virtual ~Command(); virtual std::string execute(const ParsedCommand&, Player&, GameState&) = 0; virtual std::string getHelp() const = 0; };`
- `CommandRegistry`: `void registerCommand(const std::string& verb, std::unique_ptr<Command>)`; `void addAlias(const std::string& alias, const std::string& expansion)`; `std::string expand(const std::string& normalizedLine) const` (first word replaced when it is an alias); `Command* find(const std::string& verb) const`; `std::string helpText() const` (one getHelp per line, verb order); `static void registerDefaults(CommandRegistry&)` — verbs go, look, take, drop, inventory, help, quit; aliases n/s/e/w/u/d → "go north"/…/"go up"/"go down", north/south/east/west/up/down → "go <dir>", enter → go, leave/exit/out → "go out", l → look, get → take, i/inv → inventory, q → quit.
- Messages: Go — "" on success, "Go where?", room `noExitMessage`, `canEnter` why. Look — `describe()`, or item `examine()` from room (if visible) or inventory, else "You see no <noun> here." Take — "Take what?", "It's too dark to find anything.", "There is no <noun> here.", "You can't carry any more.", "Taken: <name>." Drop — "Drop what?", "You aren't carrying <noun>.", "Dropped: <name>." Inventory — `list()` + "\nDay <n>." Help — helpText. Quit — requestQuit, "You turn your horse around."

- [ ] Tests: parser — `ParserNormalizes`, `ParserTarget` ("use key on door"), `ParserEmpty`. registry — `ExpandAliases` ("n" → "go north", "enter saloon" → "go saloon", "i" → "inventory", "look" unchanged), `FindUnknownNull`, `HelpListsAll` (7 lines). commands (small hand-built world) — `GoMoves`, `GoNothing`, `GoUnknownExit` ("sal"), `GoLockedBlocked`, `LoopMessage`, `LookRoom`, `LookItem`, `LookMissing`, `TakeAndDrop`, `TakeInDarkRoom`, `TakeWhenFull` (item stays in room), `DropMissing`, `InventoryShowsDay`, `QuitSetsFlag`.
- [ ] Fail → implement → pass → commit.

### Task 7: Game loop

**Files:** Create `include/Game.hpp`, `src/Game.cpp`, `test/FakeIO.hpp` (FakeDisplay recording calls, FakeInput from a vector), `test/testGame.cpp`; Modify `include/IDisplay.hpp` (+ `virtual void prompt(const std::string& text) = 0;`), `include/ConsoleDisplay.hpp`, `src/ConsoleDisplay.cpp` (prompt writes without newline and flushes), `test/testConsoleIO.cpp` (+ `PromptNoNewline`)

**Produces:** `Game(World&, Player&, GameState&, CommandRegistry&, const SceneLibrary&, IDisplay&, IInput&, ColourMode)`; `void run()`; `bool processTurn(const std::string& line)` (false once quit); `const std::string& lastMessage() const`; `void showScreen()` — clear, showArt(renderScene(scenes.sceneFor(room id, room kind), day, mode)), show(room name upper-cased), show(describe()), show(lastMessage) if non-empty, prompt("> ").
Empty line → "Type a command, or help." Unknown verb → `I don't understand "<line>". Type help.`

- [ ] Tests: `StartsOnDesertRoad` (screen shows "DESERT ROAD"); `WalkIntoSaloonTakeCup` (["w", "enter saloon", "take cup", "i"] → last message contains "tin cup"); `LeaveReturnsToMainStreet`; `UnknownCommand`; `EmptyLine`; `QuitAnyCase` ("  QUIT " → run ends); `EofEnds` (no input → run returns); `ClearsAndDrawsEachTurn` (clear count == screens).
- [ ] Fail → implement → pass → commit.

### Task 8: main.cpp, --plain, retire top-down code

**Files:** Modify `src/project/main.cpp` (build world, registry defaults, SceneLibrary from `data/art`, Player(start, 6), ColourMode Plain if argv has `--plain`, run). Delete `include/{SceneMap,TownLayout,Building,Entity,Viewport,Renderer,Movement,AsciiArtLibrary}.hpp`, `src/{SceneMap,TownLayout,Viewport,Renderer,Movement,AsciiArtLibrary}.cpp`, their tests, `test/data/art/`, `data/art/main_street.txt`; keep `data/art/.gitkeep`.

- [ ] `make -f Makefile.local clean playGame run-tests` → all pass.
- [ ] Smoke: `printf 'w\nenter saloon\ntake cup\ni\nleave\nsouth\nsouth\neast\nquit\n' | ./playGame --plain` → reaches THE LINE, no escape codes, exits 0; same without `--plain` shows `48;5;` codes.
- [ ] Commit.

### Task 9: Verify

- [ ] memcheck 0 errors; coverage > 90% lines on src/ excl. project; cppcheck clean; no line > 80 cols.
- [ ] Final whole-branch review; fix Critical/Important test-first.
