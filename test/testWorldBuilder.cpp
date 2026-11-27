#include <gtest/gtest.h>

#include <memory>
#include <queue>
#include <set>
#include <string>
#include <vector>

#include "DarkRoom.hpp"
#include "HazardRoom.hpp"
#include "LockedRoom.hpp"
#include "LoopRoom.hpp"
#include "Room.hpp"
#include "StandardRoom.hpp"
#include "World.hpp"
#include "WorldBuilder.hpp"

namespace {

template <typename T>
int countType(const World& world) {
  int n = 0;
  const std::vector<Room*> rooms = world.rooms();
  for (std::vector<Room*>::size_type i = 0; i < rooms.size(); ++i) {
    if (dynamic_cast<T*>(rooms[i]) != nullptr) {
      ++n;
    }
  }
  return n;
}

}  // namespace

TEST(WorldBuilderTest, EighteenRooms) {
  std::unique_ptr<World> world = WorldBuilder::build();
  EXPECT_EQ(18, world->roomCount());
}

TEST(WorldBuilderTest, TypeCounts) {
  std::unique_ptr<World> world = WorldBuilder::build();
  EXPECT_EQ(10, countType<StandardRoom>(*world));
  EXPECT_EQ(3, countType<DarkRoom>(*world));
  EXPECT_EQ(1, countType<LockedRoom>(*world));
  EXPECT_EQ(3, countType<HazardRoom>(*world));
  EXPECT_EQ(1, countType<LoopRoom>(*world));
}

TEST(WorldBuilderTest, AllReachableAndReturnable) {
  std::unique_ptr<World> world = WorldBuilder::build();
  Room* start = world->getStartRoom();
  ASSERT_NE(nullptr, start);
  EXPECT_EQ("desert_road", start->getId());

  std::set<Room*> seen;
  std::queue<Room*> frontier;
  frontier.push(start);
  seen.insert(start);
  while (!frontier.empty()) {
    Room* here = frontier.front();
    frontier.pop();
    const std::vector<std::string> names = here->exitNames();
    for (std::vector<std::string>::size_type i = 0; i < names.size(); ++i) {
      Room* next = here->getExit(names[i]);
      ASSERT_NE(nullptr, next);
      // Every exit must have a way back to where it came from.
      bool returns = false;
      const std::vector<std::string> back = next->exitNames();
      for (std::vector<std::string>::size_type j = 0; j < back.size(); ++j) {
        returns = returns || next->getExit(back[j]) == here;
      }
      EXPECT_TRUE(returns) << here->getId() << " -> " << next->getId();
      if (seen.insert(next).second) {
        frontier.push(next);
      }
    }
  }
  EXPECT_EQ(18u, seen.size());
}

TEST(WorldBuilderTest, TinCupInSaloon) {
  std::unique_ptr<World> world = WorldBuilder::build();
  Room* saloon = world->getRoom("saloon");
  ASSERT_NE(nullptr, saloon);
  EXPECT_NE(nullptr, saloon->findItem("cup"));
}

TEST(WorldBuilderTest, EvidenceRoomEnterable) {
  std::unique_ptr<World> world = WorldBuilder::build();
  Room* evidence = world->getRoom("evidence_room");
  ASSERT_NE(nullptr, evidence);
  std::string why;
  EXPECT_TRUE(evidence->canEnter(why));
}

TEST(WorldBuilderTest, StartIsDesertRoad) {
  std::unique_ptr<World> world = WorldBuilder::build();
  EXPECT_EQ(world->getRoom("desert_road"), world->getStartRoom());
}
