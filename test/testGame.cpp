#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "CommandRegistry.hpp"
#include "FakeIO.hpp"
#include "Game.hpp"
#include "GameState.hpp"
#include "Player.hpp"
#include "SceneLibrary.hpp"
#include "SceneRenderer.hpp"
#include "World.hpp"
#include "WorldBuilder.hpp"

namespace {

struct Harness {
  std::unique_ptr<World> world;
  CommandRegistry reg;
  SceneLibrary scenes;
  GameState state;
  FakeDisplay display;

  Harness() : world(WorldBuilder::build()) {
    CommandRegistry::registerDefaults(reg);
  }

  /** Plays the given lines and returns the player at the end. */
  Player play(const std::vector<std::string>& lines) {
    Player player(world->getStartRoom(), 6);
    FakeInput input(lines);
    Game game(*world, player, state, reg, scenes, display, input, ColourMode::Plain);
    game.run();
    return player;
  }
};

}  // namespace

TEST(GameTest, StartsOnDesertRoad) {
  Harness h;
  h.play({"quit"});
  EXPECT_NE(std::string::npos, h.display.all().find("DESERT ROAD"));
}

TEST(GameTest, WalkIntoSaloonTakeCup) {
  Harness h;
  Player p = h.play({"w", "enter saloon", "take cup", "i", "quit"});
  EXPECT_NE(std::string::npos, h.display.all().find("Taken: tin cup."));
  EXPECT_NE(std::string::npos, h.display.all().find("You are carrying tin cup."));
  EXPECT_EQ(1, p.getInventory().size());
}

TEST(GameTest, LeaveReturnsToMainStreet) {
  Harness h;
  Player p = h.play({"w", "enter saloon", "leave", "quit"});
  EXPECT_EQ("main_street", p.getCurrentRoom()->getId());
}

TEST(GameTest, UnknownCommand) {
  Harness h;
  h.play({"dance", "quit"});
  EXPECT_NE(std::string::npos,
            h.display.all().find("I don't understand \"dance\". Type help."));
}

TEST(GameTest, EmptyLine) {
  Harness h;
  h.play({"", "quit"});
  EXPECT_NE(std::string::npos, h.display.all().find("Type a command, or help."));
}

TEST(GameTest, QuitAnyCase) {
  Harness h;
  h.play({"  QUIT "});
  EXPECT_TRUE(h.state.quitRequested());
  EXPECT_EQ(1, h.display.clears);
}

TEST(GameTest, EofEnds) {
  Harness h;
  h.play({});
  EXPECT_TRUE(h.state.quitRequested());
}

TEST(GameTest, ClearsAndDrawsEachTurn) {
  Harness h;
  h.play({"look", "quit"});
  // One screen at the start, one after "look"; "quit" ends without a redraw.
  EXPECT_EQ(2, h.display.clears);
}
