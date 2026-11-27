#include <gtest/gtest.h>

#include "Movement.hpp"
#include "SceneMap.hpp"

namespace {

SceneMap wallMap() {
  SceneMap map("Test", 5, 3);
  map.addBuilding(Building("wall", "W", Rect(2, 0, 1, 3), Point(2, 2)));
  return map;
}

}  // namespace

TEST(MovementTest, ParsesMoveInput) {
  EXPECT_EQ(Direction::North, parseDirection("  N "));
  EXPECT_EQ(Direction::North, parseDirection("north"));
  EXPECT_EQ(Direction::South, parseDirection("S"));
  EXPECT_EQ(Direction::South, parseDirection("South"));
  EXPECT_EQ(Direction::East, parseDirection("e"));
  EXPECT_EQ(Direction::East, parseDirection("EAST"));
  EXPECT_EQ(Direction::West, parseDirection("w"));
  EXPECT_EQ(Direction::West, parseDirection("west\t"));
  EXPECT_EQ(Direction::None, parseDirection(""));
  EXPECT_EQ(Direction::None, parseDirection("   "));
  EXPECT_EQ(Direction::None, parseDirection("dance"));
}

TEST(MovementTest, StepDirections) {
  EXPECT_TRUE(step(Point(1, 1), Direction::North) == Point(1, 0));
  EXPECT_TRUE(step(Point(1, 1), Direction::South) == Point(1, 2));
  EXPECT_TRUE(step(Point(1, 1), Direction::East) == Point(2, 1));
  EXPECT_TRUE(step(Point(1, 1), Direction::West) == Point(0, 1));
  EXPECT_TRUE(step(Point(1, 1), Direction::None) == Point(1, 1));
}

TEST(MovementTest, MoveOk) {
  SceneMap map = wallMap();
  Point pos(0, 0);
  EXPECT_TRUE(tryMove(map, pos, Direction::East));
  EXPECT_TRUE(pos == Point(1, 0));
}

TEST(MovementTest, MoveBlockedAtEdge) {
  SceneMap map = wallMap();
  Point pos(0, 0);
  EXPECT_FALSE(tryMove(map, pos, Direction::North));
  EXPECT_FALSE(tryMove(map, pos, Direction::West));
  EXPECT_TRUE(pos == Point(0, 0));
}

TEST(MovementTest, MoveBlockedByWall) {
  SceneMap map = wallMap();
  Point pos(1, 0);
  EXPECT_FALSE(tryMove(map, pos, Direction::East));
  EXPECT_TRUE(pos == Point(1, 0));
}

TEST(MovementTest, MoveOntoDoor) {
  SceneMap map = wallMap();
  Point pos(1, 2);
  EXPECT_TRUE(tryMove(map, pos, Direction::East));
  EXPECT_TRUE(pos == Point(2, 2));
}

TEST(MovementTest, NoneDoesNotMove) {
  SceneMap map = wallMap();
  Point pos(0, 0);
  EXPECT_FALSE(tryMove(map, pos, Direction::None));
  EXPECT_TRUE(pos == Point(0, 0));
}
