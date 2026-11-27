#include <gtest/gtest.h>

#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "CommandRegistry.hpp"
#include "ExteriorGame.hpp"
#include "GameState.hpp"
#include "IKeyInput.hpp"
#include "IScreen.hpp"
#include "Key.hpp"
#include "LockedRoom.hpp"
#include "Player.hpp"
#include "Room.hpp"
#include "SceneKind.hpp"
#include "World.hpp"
#include "WorldBuilder.hpp"
#include "WorldFiles.hpp"
#include "WorldMap.hpp"

namespace {

class FakeKeys : public IKeyInput {
 public:
  explicit FakeKeys(const std::vector<Key>& keys) : keys_(keys), next_(0) {}
  Key readKey() override {
    return next_ < keys_.size() ? keys_[next_++] : Key(KeyType::Eof);
  }

 private:
  std::vector<Key> keys_;
  std::vector<Key>::size_type next_;
};

class FakeScreen : public IScreen {
 public:
  FakeScreen(int cols, int rows) : cols_(cols), rows_(rows) {}
  void write(const std::string& text) override { writes.push_back(text); }
  bool size(int& cols, int& rows) override {
    cols = cols_;
    rows = rows_;
    return true;
  }
  std::string all() const {
    std::string out;
    for (const std::string& w : writes) {
      out += w;
    }
    return out;
  }
  std::vector<std::string> writes;

 private:
  int cols_;
  int rows_;
};

std::vector<Key> typed(const std::string& text) {
  std::vector<Key> keys;
  for (char c : text) {
    keys.push_back(Key(KeyType::Char, c));
  }
  keys.push_back(Key(KeyType::Enter));
  return keys;
}

std::vector<Key> repeat(KeyType type, int n) {
  return std::vector<Key>(n, Key(type));
}

std::vector<Key> join(std::vector<Key> a, const std::vector<Key>& b) {
  a.insert(a.end(), b.begin(), b.end());
  return a;
}

struct Harness {
  WorldMap map;
  std::unique_ptr<World> world;
  CommandRegistry registry;
  GameState state;
  Player player;
  FakeScreen screen;

  Harness(int cols = 100, int rows = 30)
      : map(loadWorldQuietly()),
        world(WorldBuilder::build()),
        player(world->getStartRoom(), 6),
        screen(cols, rows) {
    CommandRegistry::registerDefaults(registry);
  }

  static WorldMap loadWorldQuietly() {
    std::ostringstream warn;
    return loadWorld("data/world", warn);
  }

  ExteriorGame* play(const std::vector<Key>& keys, bool plain = false) {
    keySource.reset(new FakeKeys(keys));
    game.reset(new ExteriorGame(map, *world, player, state, registry, *keySource,
                                screen, plain));
    game->run();
    return game.get();
  }

  std::unique_ptr<FakeKeys> keySource;
  std::unique_ptr<ExteriorGame> game;
};

}  // namespace

TEST(ExteriorGameTest, StartsAtMapStartFacingWest) {
  Harness h;
  ExteriorGame* g = h.play({});
  EXPECT_NEAR(h.map.start().x, g->camera().x(), 1e-9);
  EXPECT_LT(g->camera().dirX(), -0.99);
  EXPECT_NE(std::string::npos, h.screen.all().find("DESERT ROAD"));
}

TEST(ExteriorGameTest, ArrowsMoveAndTurn) {
  Harness h;
  ExteriorGame* g = h.play({Key(KeyType::Up), Key(KeyType::Up)});
  EXPECT_NEAR(h.map.start().x - 2.0, g->camera().x(), 1e-6);  // riding
  Harness t;
  ExteriorGame* turned = t.play({Key(KeyType::Left), Key(KeyType::Down)});
  EXPECT_LT(turned->camera().angle(), 3.14159);
}

TEST(ExteriorGameTest, TypingRunsLook) {
  Harness h;
  ExteriorGame* g = h.play(typed("look"));
  EXPECT_NE(std::string::npos, g->lastMessage().find("rutted road"));
  EXPECT_NE(std::string::npos, g->lastMessage().find("facing west"));
  EXPECT_EQ("", g->inputLine());
}

TEST(ExteriorGameTest, BackspaceEdits) {
  Harness h;
  std::vector<Key> keys = {Key(KeyType::Char, 'l'), Key(KeyType::Char, 'x'),
                           Key(KeyType::Backspace), Key(KeyType::Backspace),
                           Key(KeyType::Backspace)};
  ExteriorGame* g = h.play(join(keys, typed("look")));
  EXPECT_NE(std::string::npos, g->lastMessage().find("rutted road"));
}

TEST(ExteriorGameTest, UnknownCommand) {
  Harness h;
  ExteriorGame* g = h.play(typed("dance"));
  EXPECT_EQ("I don't understand \"dance\". Type help.", g->lastMessage());
}

TEST(ExteriorGameTest, EmptyEnter) {
  Harness h;
  ExteriorGame* g = h.play({Key(KeyType::Enter)});
  EXPECT_EQ("Type a command, or help.", g->lastMessage());
}

TEST(ExteriorGameTest, GoTellsYouToUseArrows) {
  Harness h;
  ExteriorGame* g = h.play(typed("go north"));
  EXPECT_EQ("Use the arrow keys to walk.", g->lastMessage());
}

TEST(ExteriorGameTest, HelpListsCommands) {
  Harness h;
  ExteriorGame* g = h.play(typed("help"));
  EXPECT_NE(std::string::npos, g->lastMessage().find("Arrow keys"));
  EXPECT_NE(std::string::npos, g->lastMessage().find("inventory"));
}

TEST(ExteriorGameTest, DelegatesInventory) {
  Harness h;
  ExteriorGame* g = h.play(typed("inventory"));
  EXPECT_NE(std::string::npos, g->lastMessage().find("You are carrying nothing."));
}

TEST(ExteriorGameTest, QuitEnds) {
  Harness h;
  h.play(join(typed("  QUIT "), {Key(KeyType::Up)}));
  EXPECT_TRUE(h.state.quitRequested());
}

TEST(ExteriorGameTest, EofEnds) {
  Harness h;
  h.play({});
  SUCCEED();
}

TEST(ExteriorGameTest, TooSmallTerminal) {
  Harness h(40, 10);
  h.play({Key(KeyType::Up)});
  EXPECT_NE(std::string::npos, h.screen.all().find("Make the terminal bigger"));
}

TEST(ExteriorGameTest, TooSmallIgnoresMovement) {
  Harness h(40, 10);
  ExteriorGame* g = h.play({Key(KeyType::Up), Key(KeyType::Left)});
  EXPECT_NEAR(h.map.start().x, g->camera().x(), 1e-9);
  EXPECT_NEAR(3.14159265, g->camera().angle(), 1e-6);
}

TEST(ExteriorGameTest, ZoneChangeUpdatesRoom) {
  Harness h;
  EXPECT_EQ("desert_road", h.player.getCurrentRoom()->getId());
  h.play(repeat(KeyType::Up, 50));
  EXPECT_EQ("main_street", h.player.getCurrentRoom()->getId());
  EXPECT_NE(std::string::npos, h.screen.all().find("MAIN STREET"));
}

TEST(ExteriorGameTest, LaterFramesAreDiffs) {
  Harness h;
  h.play({Key(KeyType::None)});
  ASSERT_GE(h.screen.writes.size(), 2u);
  EXPECT_LT(h.screen.writes[1].size(), h.screen.writes[0].size() / 4);
}

TEST(ExteriorGameTest, PlainModeHasNoColour) {
  Harness h;
  h.play({Key(KeyType::Up)}, true);
  EXPECT_EQ(std::string::npos, h.screen.all().find("38;2;"));
}

TEST(ExteriorGameTest, StartsRiding) {
  Harness h;
  ExteriorGame* g = h.play({Key(KeyType::Up)});
  EXPECT_TRUE(g->horse().isMounted());
  EXPECT_NEAR(h.map.start().x - 1.0, g->camera().x(), 1e-6);  // two half-steps
  EXPECT_NE(std::string::npos, h.screen.all().find("riding"));
}

TEST(ExteriorGameTest, DismountThenWalkSlower) {
  Harness h;
  ExteriorGame* g = h.play(join(typed("dismount"), {Key(KeyType::Up)}));
  EXPECT_FALSE(g->horse().isMounted());
  EXPECT_EQ("You swing down from the saddle.", g->lastMessage());
  EXPECT_NEAR(h.map.start().x - 0.5, g->camera().x(), 1e-6);
  EXPECT_NE(std::string::npos, h.screen.all().find("on foot"));
}

TEST(ExteriorGameTest, MountNeedsHorseNearby) {
  Harness h;
  std::vector<Key> keys = typed("dismount");
  keys = join(keys, repeat(KeyType::Up, 8));
  keys = join(keys, typed("mount"));
  ExteriorGame* g = h.play(keys);
  EXPECT_EQ("Your horse is too far away.", g->lastMessage());
  Harness back;
  ExteriorGame* again = back.play(join(typed("dismount"), typed("ride horse")));
  EXPECT_TRUE(again->horse().isMounted());
}

TEST(ExteriorGameTest, TimePassesWhenYouAct) {
  Harness h;
  ExteriorGame* g = h.play(join({Key(KeyType::Up), Key(KeyType::Left)}, typed("look")));
  EXPECT_EQ("17:32", g->clock().format());  // step 2, turn and look free
  Harness w;
  ExteriorGame* waited = w.play(typed("wait"));
  EXPECT_EQ("18:30", waited->clock().format());
  EXPECT_NE(std::string::npos, w.screen.all().find("18:30"));
  Harness d;
  ExteriorGame* dismounted = d.play(typed("dismount"));
  EXPECT_EQ("17:35", dismounted->clock().format());
}

TEST(ExteriorGameTest, BlockedStepCostsNothing) {
  Harness h;
  // Turn to face the open desert edge east and walk until blocked by the map edge.
  std::vector<Key> keys = repeat(KeyType::Right, 12);  // 180 degrees: face east
  keys = join(keys, repeat(KeyType::Up, 8));           // reach and settle at the edge
  ExteriorGame* g = h.play(keys);
  const std::string after = g->clock().format();
  Harness again;
  keys = join(keys, repeat(KeyType::Up, 5));           // bump the edge
  EXPECT_EQ(after, again.play(keys)->clock().format());
}

TEST(ExteriorGameTest, WaitingPastMidnightAdvancesDay) {
  Harness h;
  std::vector<Key> keys;
  for (int i = 0; i < 7; ++i) {
    keys = join(keys, typed("wait"));
  }
  ExteriorGame* g = h.play(keys);
  EXPECT_EQ("00:30", g->clock().format());
  EXPECT_EQ(2, h.state.getDay());
}

namespace {

/** Rides west to the sheriff's door, then faces it (north). */
std::vector<Key> rideToSheriff() {
  std::vector<Key> keys = repeat(KeyType::Up, 33);   // riding: x 60.5 -> 27.5
  keys = join(keys, repeat(KeyType::Right, 6));       // west -> north
  return join(keys, repeat(KeyType::Up, 2));          // y 17.5 -> 15.5
}

}  // namespace

TEST(ExteriorGameTest, EnterNeedsADoor) {
  Harness h;
  ExteriorGame* g = h.play(join(typed("dismount"), typed("enter")));
  EXPECT_EQ("There's no door here.", g->lastMessage());
  EXPECT_FALSE(g->isInside());
}

TEST(ExteriorGameTest, EnterOnHorsebackRefused) {
  Harness h;
  ExteriorGame* g = h.play(join(rideToSheriff(), typed("enter")));
  EXPECT_EQ("Dismount first.", g->lastMessage());
  EXPECT_FALSE(g->isInside());
}

TEST(ExteriorGameTest, EnterAndMoveInside) {
  Harness h;
  std::vector<Key> keys = join(rideToSheriff(), typed("dismount"));
  keys = join(keys, typed("enter sheriff"));
  ExteriorGame* g = h.play(keys);
  EXPECT_TRUE(g->isInside());
  EXPECT_EQ("sheriffs_office", h.player.getCurrentRoom()->getId());
  EXPECT_NE(std::string::npos, h.screen.all().find("SHERIFF'S OFFICE"));

  Harness deeper;
  keys = join(rideToSheriff(), typed("dismount"));
  keys = join(keys, typed("enter"));
  keys = join(keys, typed("north"));
  deeper.play(keys);
  EXPECT_EQ("evidence_room", deeper.player.getCurrentRoom()->getId());
}

TEST(ExteriorGameTest, WrongPlaceNamed) {
  Harness h;
  std::vector<Key> keys = join(rideToSheriff(), typed("dismount"));
  ExteriorGame* g = h.play(join(keys, typed("enter saloon")));
  EXPECT_EQ("You're not at the saloon.", g->lastMessage());
}

TEST(ExteriorGameTest, LeaveReturnsOutsideFacingAway) {
  Harness h;
  std::vector<Key> keys = join(rideToSheriff(), typed("dismount"));
  keys = join(keys, typed("enter"));
  keys = join(keys, typed("north"));
  keys = join(keys, typed("leave"));
  ExteriorGame* g = h.play(keys);
  EXPECT_FALSE(g->isInside());
  EXPECT_EQ("main_street", h.player.getCurrentRoom()->getId());
  EXPECT_NEAR(27.5, g->camera().x(), 1e-6);
  EXPECT_NEAR(15.5, g->camera().y(), 1e-6);
  EXPECT_GT(g->camera().dirY(), 0.99);  // facing south, away from the door
}

TEST(ExteriorGameTest, ArrowsInsideExplainLeave) {
  Harness h;
  std::vector<Key> keys = join(rideToSheriff(), typed("dismount"));
  keys = join(keys, typed("enter"));
  keys.push_back(Key(KeyType::Up));
  ExteriorGame* g = h.play(keys);
  EXPECT_TRUE(g->isInside());
  EXPECT_EQ("You're indoors. Type leave to go back outside.", g->lastMessage());
}

TEST(ExteriorGameTest, InsideLookAndTake) {
  Harness h;
  std::vector<Key> keys = join(rideToSheriff(), typed("dismount"));
  keys = join(keys, typed("enter"));
  keys = join(keys, typed("look"));
  ExteriorGame* g = h.play(keys);
  EXPECT_NE(std::string::npos, g->lastMessage().find("wanted board"));
}

TEST(ExteriorGameTest, LockedDoorStaysShut) {
  WorldMap map;
  std::ostringstream warn;
  parseTownMap("......\n.aaa..\n..@...\n", map);
  parseBuildings("a vault \"VAULT\" 1.2 planks 1,1,1 2,2,2 2,2 S evidence_room\n",
                 map, warn);
  World world;
  Room* vault = world.addRoom(std::unique_ptr<Room>(new LockedRoom(
      "evidence_room", "Evidence Room", "Files.", SceneKind::Indoor, "iron_key")));
  world.setStartRoom(vault);
  CommandRegistry registry;
  CommandRegistry::registerDefaults(registry);
  GameState state;
  Player player(vault, 6);
  FakeKeys keys(join(typed("dismount"), typed("enter")));
  FakeScreen screen(100, 30);
  ExteriorGame g(map, world, player, state, registry, keys, screen, false);
  g.run();
  EXPECT_EQ("The door is locked. You need a key.", g.lastMessage());
  EXPECT_FALSE(g.isInside());
}

TEST(ExteriorGameTest, LongInputIsCapped) {
  Harness h;
  ExteriorGame* g = h.play(std::vector<Key>(200, Key(KeyType::Char, 'a')));
  EXPECT_LE(g->inputLine().size(), 60u);
}
