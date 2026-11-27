```
 _____ _   _ _____    _     ___ _   _ _____
|_   _| | | | ____|  | |   |_ _| \ | | ____|
  | | | |_| |  _|    | |    | ||  \| |  _|
  | | |  _  | |___   | |___ | || |\  | |___
  |_| |_| |_|_____|  |_____|___|_| \_|_____|

```

> *It doesn't hate you. You just aren't supposed to be here.*

**The Line** is a text adventure set in the old West that slowly turns into cosmic horror. You play it in a terminal, with ASCII art in the style of the games that came before graphics.

You're an outlaw riding into a dusty town on the edge of Arizona, near the Mojave. You don't remember what you did. Nobody here has heard of you yet. You take a room at the hotel, lie down, and close your eyes.

In the morning, there is a line in the sand.

---

## Features

| | |
|---|---|
| **A western that turns into cosmic horror** | It starts as a quiet frontier town. It doesn't stay that way. |
| **ASCII art that falls apart** | Every location has its own art. As the line gets closer, the art loses characters and reality gets thinner. |
| **18 locations** | Saloon, hotel, gunsmith, sheriff's office, stable, church, graveyard, telegraph office, an old mine, a dry well, and the lakebed. |
| **8 puzzles** | Dig up your own past, arm yourself, and work out what's on the other side of the line. |
| **10 random events** | They start strange and end with the town turning on itself. No two runs play the same. |
| **A real countdown** | You have seven days, and the piano player's song gets one note shorter each day. |
| **5 endings** | Only one of them is a win, and it isn't a happy one. |
| **Fights** | When the saloon breaks out in a brawl, you can throw punches. Or keep your last bullet for something worse. |

---

## Building and running

The project uses C++ and Make.

```bash
make playGame     # build the game
./playGame        # play it

make testGame     # build the unit tests
./testGame        # run them

make memcheck     # check for memory leaks
make coverage     # generate a code coverage report
```

---

## How to play

Type commands in plain English. Upper or lower case doesn't matter.

| Command | What it does | Example |
|---|---|---|
| `go <direction>` | Move north, south, east, west, up or down | `go north`, `n` |
| `look` / `look <thing>` | Look around, or examine something | `look poster` |
| `take <item>` | Pick something up | `take lantern` |
| `drop <item>` | Put something down | `drop apple` |
| `use <item>` / `use <item> on <target>` | Use an item | `use key on door` |
| `talk <person>` | Talk to someone | `talk piano player` |
| `give <item> to <person>` | Hand something over | `give apple to horse` |
| `fight <person>` | Fight someone | `fight brawler` |
| `sleep` | Rest at the hotel | `sleep` |
| `inventory` / `i` | Check what you're carrying, your health, your sanity and the day | `i` |
| `help` | List all commands | `help` |
| `quit` | Give up | `quit` |

Keep an eye on two numbers. **Health** drops when something hurts you. **Sanity** drops when you see things you shouldn't. If either reaches zero, the game is over.

---

## The town

```
   ___________________________________________
  |  HOTEL  | SALOON |GUNSMITH| SHERIFF|STABLE|
  |_________|________|________|________|______|
  ========== M A I N   S T R E E T ===========
   CHURCH   GENERAL STORE   TELEGRAPH   GRAVES
        . . . . . . . . . . . . . . . . . .
              (the desert road east)
```

People here are mostly friendly. Some of them are a little *off*. The bartender has no reflection. The preacher talks about a God who *measures*. The deputy stands at the edge of town facing the lakebed, and he never blinks.

Talk to everyone early. People don't always stay.

---

<details>
<summary><b>Spoilers: the story behind the line</b> (click to open)</summary>

<br>

Out on Soda Lake, something is correcting a map. It has no name, and nobody in town has seen it. This town was never supposed to be on it. Everything past the line is erased: no sand, no sky, only a flat white.

You're the reason it started. A railroad company paid you to dig up an old brass survey marker from the lakebed. Nobody human ever set that marker. It was the pin holding the map in place, and it's still in your saddlebag. The erasure is eating your memory too, so you'll need to find your own wanted poster to remember.

**The endings**
- **The Line Is Kept:** put the marker back. You finally see what has been drawing the line, and it draws you out of the map. The town survives, and you don't.
- **Unmapped:** on day seven the line reaches town, and you are left in the void outside existence.
- **Hollow:** the deputy gets you.
- **Unmoored:** your mind gives out.
- **The Long Ride:** leave before you ever sleep in the hotel. The road lets you go. You never notice the line following you, and half of the American Southwest is wiped off the map.

</details>

---

## Project structure

```
include/        header files (class declarations and documentation)
src/            implementation files
src/project/    main.cpp
test/           unit tests
data/art/       ASCII art, one .txt file per scene
Docs/Design/    Design_Report_GroupF.pdf (UML class diagrams and method descriptions)
```

The design is built around SOLID principles and polymorphism. There's an abstract `Room` with five room types, plus abstract `Item`, `NPC`, `Command` and `IRandomEvent` hierarchies. Interfaces for input, output and random numbers make the core game logic fully unit testable. The full design is in [`Docs/Design/Design_Report_GroupF.pdf`](Docs/Design/Design_Report_GroupF.pdf).

---

## Design Phase

### Rowan Feland
* Created the game theme, story, plot and all five endings (The Line)
* Designed the world: the Room hierarchy (StandardRoom, DarkRoom, LockedRoom, HazardRoom, LoopRoom), World and WorldBuilder
* Wrote the room contents, clues and uncanny details for all 18 rooms
* Designed the ASCII art display and day based art erosion (AsciiArtLibrary, IDisplay, ConsoleDisplay)

### Zeynep Camgoz
* Designed the Character hierarchy (Player, NPC, HelperNPC, HostileNPC)
* Designed Inventory, the Item hierarchy and the IUsable interface
* Designed the random event system (EventScheduler, IRandom, IRandomEvent and the ten events)
* Drew the UML class diagrams

### Heather-May Howse
* Designed the Command hierarchy (including Sleep and Fight), CommandRegistry and CommandParser
* Designed GameState, GameEndChecker and the ILoseCondition classes
* Wrote the method description table
* Wrote the SOLID and clean code design notes

---

<p align="center"><i>CPSC 2720 · University of Lethbridge · Fall 2026 · Group F</i></p>

```
 . . . . . . . . . . . . . . . . . . . . . . . .
 ________________________________________________
                 don't cross it.
```
