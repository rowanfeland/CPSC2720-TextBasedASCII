#include "WorldBuilder.hpp"

#include <memory>
#include <string>

#include "DarkRoom.hpp"
#include "HazardRoom.hpp"
#include "Item.hpp"
#include "LockedRoom.hpp"
#include "LoopRoom.hpp"
#include "Room.hpp"
#include "SceneKind.hpp"
#include "StandardRoom.hpp"
#include "World.hpp"

namespace {

Room* addStandard(World& w, const std::string& id, const std::string& name,
                  const std::string& desc, SceneKind kind) {
  return w.addRoom(std::unique_ptr<Room>(new StandardRoom(id, name, desc, kind)));
}

Room* addDark(World& w, const std::string& id, const std::string& name,
              const std::string& desc, SceneKind kind) {
  return w.addRoom(std::unique_ptr<Room>(new DarkRoom(id, name, desc, kind)));
}

Room* addHazard(World& w, const std::string& id, const std::string& name,
                const std::string& desc, SceneKind kind, const std::string& unease) {
  return w.addRoom(std::unique_ptr<Room>(new HazardRoom(id, name, desc, kind, unease)));
}

}  // namespace

std::unique_ptr<World> WorldBuilder::build() {
  std::unique_ptr<World> w(new World());

  Room* street = addStandard(*w, "main_street", "Main Street",
      "Main Street runs east to west past false fronts. Dust hangs in the still air.",
      SceneKind::Outdoor);
  Room* desert = w->addRoom(std::unique_ptr<Room>(new LoopRoom("desert_road",
      "Desert Road", "A rutted road runs east into flat white desert, and back toward town.",
      SceneKind::Outdoor, "east", "The road east shimmers. You're not ready to leave yet.")));
  Room* graves = addHazard(*w, "graveyard", "Graveyard",
      "Wooden crosses lean in the dry grass. Some of the graves are fresh.",
      SceneKind::Outdoor, "The grass over the newest grave is already grown over.");
  Room* lake = addHazard(*w, "soda_lake", "Soda Lake",
      "A flat white expanse of dried lake. Your boots crunch on salt.",
      SceneKind::Outdoor, "The horizon out here has no edge.");
  Room* line = addHazard(*w, "the_line", "The Line",
      "Past this point there is no sand, no sky, only a flat white.",
      SceneKind::Outdoor, "You can't make out the ground beyond your boots.");

  Room* lobby = addStandard(*w, "hotel_lobby", "Hotel Lobby",
      "A small lobby with a dusty desk and a bell. Stairs climb up, and a door leads down.",
      SceneKind::Indoor);
  Room* hotelRoom = addStandard(*w, "hotel_room", "Hotel Room",
      "A narrow room with an iron bed. The window looks out over Main Street.",
      SceneKind::Indoor);
  Room* cellar = addDark(*w, "hotel_cellar", "Hotel Cellar",
      "A cold cellar of stacked crates. Your footsteps echo.", SceneKind::Indoor);
  Room* saloon = addStandard(*w, "saloon", "Saloon",
      "A long bar, a piano, and tables scarred by old fights. Lanterns sway overhead.",
      SceneKind::Indoor);
  Room* gunsmith = addStandard(*w, "gunsmith", "Gunsmith",
      "Rifles on the wall, a glass case of cartridges, and the smell of gun oil.",
      SceneKind::Indoor);
  Room* sheriff = addStandard(*w, "sheriffs_office", "Sheriff's Office",
      "A desk, a cell, and a wanted board. A door at the back is marked EVIDENCE.",
      SceneKind::Indoor);
  LockedRoom* evidence = new LockedRoom("evidence_room",
      "Evidence Room", "Boxes of old files. A map on the wall has a pin missing.",
      SceneKind::Indoor, "iron_key");
  evidence->unlock("iron_key");  // Open until the iron-key puzzle phase.
  w->addRoom(std::unique_ptr<Room>(evidence));
  Room* stable = addStandard(*w, "stable", "Stable",
      "Straw on the floor, harness on the pegs, and the smell of horses.",
      SceneKind::Indoor);
  Room* store = addStandard(*w, "general_store", "General Store",
      "Shelves of tins and cloth. A ledger lies open on the counter.",
      SceneKind::Indoor);
  Room* church = addStandard(*w, "church", "Church",
      "Plain pews face the back wall. A hymn board lists the days left.",
      SceneKind::Indoor);
  Room* telegraph = addStandard(*w, "telegraph_office", "Telegraph Office",
      "A telegraph key chatters on the desk, sending nothing you can read.",
      SceneKind::Indoor);
  Room* mine = addDark(*w, "old_mine", "Old Mine",
      "A timber-framed shaft leads into the hill. The air tastes of iron.",
      SceneKind::Indoor);
  Room* well = addDark(*w, "dry_well", "Dry Well",
      "A stone well with no water, its rim worn smooth. Something glints far below.",
      SceneKind::Outdoor);

  saloon->addItem(std::unique_ptr<Item>(new Item("tin cup",
      "A dented tin cup. It smells of whiskey.")));

  w->connect(street, "east", desert, "west");
  w->connect(street, "north", mine, "south");
  w->connect(street, "west", well, "east");
  w->connect(street, "south", graves, "north");
  w->connect(graves, "south", lake, "north");
  w->connect(lake, "east", line, "west");

  w->connect(street, "hotel", lobby, "out");
  w->connect(street, "saloon", saloon, "out");
  w->connect(street, "gunsmith", gunsmith, "out");
  w->connect(street, "sheriff", sheriff, "out");
  w->connect(street, "stable", stable, "out");
  w->connect(street, "store", store, "out");
  w->connect(street, "church", church, "out");
  w->connect(street, "telegraph", telegraph, "out");

  w->connect(lobby, "up", hotelRoom, "down");
  w->connect(lobby, "down", cellar, "up");
  w->connect(sheriff, "north", evidence, "south");

  w->setStartRoom(desert);
  return w;
}
