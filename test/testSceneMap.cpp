#include <gtest/gtest.h>
#include "SceneMap.hpp"

namespace {

SceneMap smallMap() {
  SceneMap map("Test", 10, 6);
  // Box covering x 2..5, y 1..3 with its door on the bottom edge.
  map.addBuilding(Building("shed", "SHED", Rect(2, 1, 4, 3), Point(3, 3)));
  return map;
}

}  // namespace

TEST(SceneMapTest, Basics) {
  SceneMap map("Main Street", 10, 6, '.');
  EXPECT_EQ("Main Street", map.name());
  EXPECT_EQ(10, map.width());
  EXPECT_EQ(6, map.height());
  EXPECT_EQ('.', map.ground());
  EXPECT_TRUE(map.buildings().empty());
}

TEST(SceneMapTest, WalkableEdges) {
  SceneMap map = smallMap();
  EXPECT_TRUE(map.isWalkable(Point(0, 0)));
  EXPECT_TRUE(map.isWalkable(Point(9, 5)));
  EXPECT_FALSE(map.isWalkable(Point(-1, 0)));
  EXPECT_FALSE(map.isWalkable(Point(10, 0)));
  EXPECT_FALSE(map.isWalkable(Point(0, 6)));
  EXPECT_FALSE(map.isInside(Point(0, -1)));
}

TEST(SceneMapTest, FootprintBlocks) {
  SceneMap map = smallMap();
  EXPECT_FALSE(map.isWalkable(Point(2, 1)));
  EXPECT_FALSE(map.isWalkable(Point(4, 2)));
  EXPECT_FALSE(map.isWalkable(Point(5, 3)));
  EXPECT_TRUE(map.isWalkable(Point(6, 2)));
}

TEST(SceneMapTest, DoorWalkable) {
  SceneMap map = smallMap();
  EXPECT_TRUE(map.isWalkable(Point(3, 3)));
}

TEST(SceneMapTest, BuildingAtDoor) {
  SceneMap map = smallMap();
  const Building* b = map.buildingAtDoor(Point(3, 3));
  ASSERT_NE(nullptr, b);
  EXPECT_EQ("shed", b->id);
  EXPECT_EQ("SHED", b->label);
  EXPECT_EQ(nullptr, map.buildingAtDoor(Point(4, 3)));
}

TEST(SceneMapTest, Roads) {
  SceneMap map = smallMap();
  EXPECT_FALSE(map.isRoad(4));
  map.addRoad(4);
  EXPECT_TRUE(map.isRoad(4));
  EXPECT_FALSE(map.isRoad(5));
}
