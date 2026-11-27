#include <gtest/gtest.h>

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

TEST(StandardRoomTest, DescribeListsItemsAndExits) {
  StandardRoom room("saloon", "Saloon", "Smoke and sawdust.", SceneKind::Indoor);
  room.addItem(std::unique_ptr<Item>(new Item("tin cup", "A dented tin cup.")));
  Room* street = nullptr;
  StandardRoom other("street", "Main Street", "Dust.", SceneKind::Outdoor);
  street = &other;
  room.setExit("out", street);
  const std::string text = room.describe();
  EXPECT_NE(std::string::npos, text.find("Smoke and sawdust."));
  EXPECT_NE(std::string::npos, text.find("You see a tin cup."));
  EXPECT_NE(std::string::npos, text.find("Exits: out."));
}

TEST(StandardRoomTest, ExitLookupNormalized) {
  StandardRoom a("a", "A", "d", SceneKind::Indoor);
  StandardRoom b("b", "B", "d", SceneKind::Indoor);
  a.setExit("Out", &b);
  EXPECT_EQ(&b, a.getExit("  OUT "));
  EXPECT_EQ(nullptr, a.getExit("up"));
}

TEST(StandardRoomTest, DefaultsAllowEntryAndExitMessage) {
  StandardRoom a("a", "A", "d", SceneKind::Indoor);
  std::string why;
  EXPECT_TRUE(a.canEnter(why));
  EXPECT_EQ("You can't go that way.", a.noExitMessage("north"));
  EXPECT_EQ(SceneKind::Indoor, a.sceneKind());
  EXPECT_TRUE(a.itemsVisible());
}

TEST(StandardRoomTest, ItemRemoveAndFind) {
  StandardRoom a("a", "A", "d", SceneKind::Indoor);
  a.addItem(std::unique_ptr<Item>(new Item("tin cup", "d")));
  ASSERT_NE(nullptr, a.findItem("cup"));
  std::unique_ptr<Item> cup = a.removeItem("cup");
  ASSERT_NE(nullptr, cup.get());
  EXPECT_EQ(nullptr, a.findItem("cup"));
  EXPECT_EQ(nullptr, a.removeItem("cup").get());
}

TEST(DarkRoomTest, DarkHidesUntilLit) {
  DarkRoom mine("old_mine", "Old Mine", "Timber and rock.", SceneKind::Indoor);
  mine.addItem(std::unique_ptr<Item>(new Item("gold nugget", "d")));
  EXPECT_FALSE(mine.isLit());
  EXPECT_FALSE(mine.itemsVisible());
  EXPECT_EQ(SceneKind::Dark, mine.sceneKind());
  EXPECT_NE(std::string::npos, mine.describe().find("It is too dark to see."));
  EXPECT_EQ(std::string::npos, mine.describe().find("gold nugget"));

  mine.illuminate();
  EXPECT_TRUE(mine.isLit());
  EXPECT_TRUE(mine.itemsVisible());
  EXPECT_EQ(SceneKind::Indoor, mine.sceneKind());
  EXPECT_NE(std::string::npos, mine.describe().find("You see a gold nugget."));
}

TEST(LockedRoomTest, LockedBlocksUntilUnlocked) {
  LockedRoom evidence("evidence_room", "Evidence Room", "Files.", SceneKind::Indoor,
                      "iron_key");
  std::string why;
  EXPECT_TRUE(evidence.isLocked());
  EXPECT_FALSE(evidence.canEnter(why));
  EXPECT_EQ("The door is locked. You need a key.", why);
  EXPECT_FALSE(evidence.unlock("brass_key"));
  EXPECT_TRUE(evidence.isLocked());
  EXPECT_TRUE(evidence.unlock("iron_key"));
  EXPECT_FALSE(evidence.isLocked());
  EXPECT_TRUE(evidence.canEnter(why));
}

TEST(HazardRoomTest, HazardAddsUnease) {
  HazardRoom graves("graveyard", "Graveyard", "Headstones.", SceneKind::Outdoor,
                    "Something here is wrong with the light.");
  EXPECT_NE(std::string::npos,
            graves.describe().find("Something here is wrong with the light."));
  EXPECT_EQ(SceneKind::Outdoor, graves.sceneKind());
}

TEST(LoopRoomTest, LoopOnwardMessage) {
  LoopRoom road("desert_road", "Desert Road", "Dust.", SceneKind::Outdoor, "east",
                "The road east shimmers. You're not ready to leave yet.");
  EXPECT_EQ("The road east shimmers. You're not ready to leave yet.",
            road.noExitMessage("east"));
  EXPECT_EQ("You can't go that way.", road.noExitMessage("north"));
}
