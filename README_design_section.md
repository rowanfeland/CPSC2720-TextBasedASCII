## Design Phase

Design document: `docs/design/design.pdf`

### Rowan Feland
* Created the game theme, story, plot and all five endings (The Line)
* Designed the world: the Room hierarchy (StandardRoom, DarkRoom, LockedRoom, HazardRoom, LoopRoom), World and WorldBuilder
* Wrote the room contents, clues and uncanny details for all 18 rooms
* Designed the ASCII art display and day based art erosion (AsciiArtLibrary, IDisplay, ConsoleDisplay)

### Team Member 2
* Designed the Character hierarchy (Player, NPC, HelperNPC, HostileNPC)
* Designed Inventory, the Item hierarchy and the IUsable interface
* Designed the random event system (EventScheduler, IRandom, IRandomEvent and the ten events)
* Drew the UML class diagrams

### Team Member 3
* Designed the Command hierarchy (including Sleep and Fight), CommandRegistry and CommandParser
* Designed GameState, GameEndChecker and the ILoseCondition classes
* Wrote the method description table
* Wrote the SOLID and clean code design notes
