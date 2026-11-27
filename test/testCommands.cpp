#include <gtest/gtest.h>

#include <memory>
#include <string>

#include "CommandParser.hpp"
#include "CommandRegistry.hpp"
#include "Commands.hpp"
#include "DarkRoom.hpp"
#include "GameState.hpp"
#include "Item.hpp"
#include "LockedRoom.hpp"
#include "LoopRoom.hpp"
#include "Player.hpp"
#include "Room.hpp"
#include "SceneKind.hpp"
#include "StandardRoom.hpp"

namespace {

std::string run(CommandRegistry& reg, const std::string& line, Player& p, GameState& s) {
  ParsedCommand c = CommandParser::parse(reg.expand(line));
  Command* cmd = reg.find(c.verb);
  return cmd == nullptr ? std::string("?") : cmd->execute(c, p, s);
}

std::unique_ptr<Item> cup() {
  return std::unique_ptr<Item>(new Item("tin cup", "A dented tin cup."));
}

}  // namespace

TEST(CommandsTest, GoMoves) {
  StandardRoom hall("hall", "Hall", "d", SceneKind::Indoor);
  StandardRoom yard("yard", "Yard", "d", SceneKind::Outdoor);
  hall.setExit("north", &yard);
  Player p(&hall, 6);
  GameState s;
  CommandRegistry reg;
  CommandRegistry::registerDefaults(reg);
  EXPECT_EQ("", run(reg, "go north", p, s));
  EXPECT_EQ(&yard, p.getCurrentRoom());
}

TEST(CommandsTest, GoNothing) {
  StandardRoom hall("hall", "Hall", "d", SceneKind::Indoor);
  Player p(&hall, 6);
  GameState s;
  CommandRegistry reg;
  CommandRegistry::registerDefaults(reg);
  EXPECT_EQ("Go where?", run(reg, "go", p, s));
  EXPECT_EQ(&hall, p.getCurrentRoom());
}

TEST(CommandsTest, GoUnknownExit) {
  StandardRoom hall("hall", "Hall", "d", SceneKind::Indoor);
  StandardRoom saloon("saloon", "Saloon", "d", SceneKind::Indoor);
  hall.setExit("saloon", &saloon);
  Player p(&hall, 6);
  GameState s;
  CommandRegistry reg;
  CommandRegistry::registerDefaults(reg);
  EXPECT_EQ("You can't go that way.", run(reg, "go sal", p, s));
  EXPECT_EQ(&hall, p.getCurrentRoom());
}

TEST(CommandsTest, GoLockedBlocked) {
  StandardRoom hall("hall", "Hall", "d", SceneKind::Indoor);
  LockedRoom vault("vault", "Vault", "d", SceneKind::Indoor, "iron_key");
  hall.setExit("vault", &vault);
  Player p(&hall, 6);
  GameState s;
  CommandRegistry reg;
  CommandRegistry::registerDefaults(reg);
  EXPECT_EQ("The door is locked. You need a key.", run(reg, "enter vault", p, s));
  EXPECT_EQ(&hall, p.getCurrentRoom());
}

TEST(CommandsTest, LoopMessage) {
  LoopRoom road("desert_road", "Desert Road", "d", SceneKind::Outdoor, "east",
                "The road east shimmers.");
  Player p(&road, 6);
  GameState s;
  CommandRegistry reg;
  CommandRegistry::registerDefaults(reg);
  EXPECT_EQ("The road east shimmers.", run(reg, "go east", p, s));
  EXPECT_EQ(&road, p.getCurrentRoom());
}

TEST(CommandsTest, LookRoom) {
  StandardRoom hall("hall", "Hall", "A quiet hall.", SceneKind::Indoor);
  Player p(&hall, 6);
  GameState s;
  CommandRegistry reg;
  CommandRegistry::registerDefaults(reg);
  EXPECT_NE(std::string::npos, run(reg, "look", p, s).find("A quiet hall."));
}

TEST(CommandsTest, LookItem) {
  StandardRoom hall("hall", "Hall", "d", SceneKind::Indoor);
  hall.addItem(cup());
  Player p(&hall, 6);
  GameState s;
  CommandRegistry reg;
  CommandRegistry::registerDefaults(reg);
  EXPECT_EQ("A dented tin cup.", run(reg, "look cup", p, s));
}

TEST(CommandsTest, LookMissing) {
  StandardRoom hall("hall", "Hall", "d", SceneKind::Indoor);
  Player p(&hall, 6);
  GameState s;
  CommandRegistry reg;
  CommandRegistry::registerDefaults(reg);
  EXPECT_EQ("You see no rope here.", run(reg, "look rope", p, s));
}

TEST(CommandsTest, TakeAndDrop) {
  StandardRoom hall("hall", "Hall", "d", SceneKind::Indoor);
  hall.addItem(cup());
  Player p(&hall, 6);
  GameState s;
  CommandRegistry reg;
  CommandRegistry::registerDefaults(reg);
  EXPECT_EQ("Taken: tin cup.", run(reg, "take cup", p, s));
  EXPECT_EQ(1, p.getInventory().size());
  EXPECT_EQ(nullptr, hall.findItem("cup"));
  EXPECT_EQ("Dropped: tin cup.", run(reg, "drop cup", p, s));
  EXPECT_EQ(0, p.getInventory().size());
  EXPECT_NE(nullptr, hall.findItem("cup"));
}

TEST(CommandsTest, TakeInDarkRoom) {
  DarkRoom cellar("cellar", "Cellar", "d", SceneKind::Indoor);
  cellar.addItem(cup());
  Player p(&cellar, 6);
  GameState s;
  CommandRegistry reg;
  CommandRegistry::registerDefaults(reg);
  EXPECT_EQ("It's too dark to find anything.", run(reg, "take cup", p, s));
  EXPECT_NE(nullptr, cellar.findItem("cup"));
  EXPECT_EQ(0, p.getInventory().size());
}

TEST(CommandsTest, TakeWhenFull) {
  StandardRoom hall("hall", "Hall", "d", SceneKind::Indoor);
  hall.addItem(cup());
  Player p(&hall, 0);
  GameState s;
  CommandRegistry reg;
  CommandRegistry::registerDefaults(reg);
  EXPECT_EQ("You can't carry any more.", run(reg, "take cup", p, s));
  EXPECT_NE(nullptr, hall.findItem("cup"));
}

TEST(CommandsTest, TakeWithoutNoun) {
  StandardRoom hall("hall", "Hall", "d", SceneKind::Indoor);
  Player p(&hall, 6);
  GameState s;
  CommandRegistry reg;
  CommandRegistry::registerDefaults(reg);
  EXPECT_EQ("Take what?", run(reg, "take", p, s));
  EXPECT_EQ("There is no rope here.", run(reg, "take rope", p, s));
}

TEST(CommandsTest, DropMissing) {
  StandardRoom hall("hall", "Hall", "d", SceneKind::Indoor);
  Player p(&hall, 6);
  GameState s;
  CommandRegistry reg;
  CommandRegistry::registerDefaults(reg);
  EXPECT_EQ("You aren't carrying rope.", run(reg, "drop rope", p, s));
  EXPECT_EQ("Drop what?", run(reg, "drop", p, s));
}

TEST(CommandsTest, InventoryShowsDay) {
  StandardRoom hall("hall", "Hall", "d", SceneKind::Indoor);
  Player p(&hall, 6);
  GameState s;
  s.setDay(3);
  CommandRegistry reg;
  CommandRegistry::registerDefaults(reg);
  const std::string out = run(reg, "i", p, s);
  EXPECT_NE(std::string::npos, out.find("You are carrying nothing."));
  EXPECT_NE(std::string::npos, out.find("Day 3."));
}

TEST(CommandsTest, QuitSetsFlag) {
  StandardRoom hall("hall", "Hall", "d", SceneKind::Indoor);
  Player p(&hall, 6);
  GameState s;
  CommandRegistry reg;
  CommandRegistry::registerDefaults(reg);
  EXPECT_FALSE(s.quitRequested());
  run(reg, "quit", p, s);
  EXPECT_TRUE(s.quitRequested());
}
