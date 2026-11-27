#include <gtest/gtest.h>

#include <memory>

#include "Room.hpp"
#include "SceneKind.hpp"
#include "StandardRoom.hpp"
#include "World.hpp"

TEST(WorldTest, AddGetConnect) {
  World world;
  Room* a = world.addRoom(std::unique_ptr<Room>(
      new StandardRoom("a", "A", "d", SceneKind::Indoor)));
  Room* b = world.addRoom(std::unique_ptr<Room>(
      new StandardRoom("b", "B", "d", SceneKind::Indoor)));
  EXPECT_EQ(2, world.roomCount());
  EXPECT_EQ(a, world.getRoom("a"));
  world.connect(a, "north", b, "south");
  EXPECT_EQ(b, a->getExit("north"));
  EXPECT_EQ(a, b->getExit("south"));
}

TEST(WorldTest, UnknownRoomNull) {
  World world;
  EXPECT_EQ(nullptr, world.getRoom("nowhere"));
  EXPECT_EQ(nullptr, world.getStartRoom());
}

TEST(WorldTest, StartRoom) {
  World world;
  Room* a = world.addRoom(std::unique_ptr<Room>(
      new StandardRoom("a", "A", "d", SceneKind::Indoor)));
  world.setStartRoom(a);
  EXPECT_EQ(a, world.getStartRoom());
  EXPECT_EQ(1u, world.rooms().size());
}
