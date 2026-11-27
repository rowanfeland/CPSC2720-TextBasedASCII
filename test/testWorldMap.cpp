#include <gtest/gtest.h>

#include <sstream>
#include <string>

#include "WorldFiles.hpp"
#include "WorldMap.hpp"

namespace {

const char kMap[] =
    "#######\n"
    "#..aa.#\n"
    "#..aa.#\n"
    "#==@==#\n"
    "#,~F_.#\n"
    "#######\n";

const char kBuildings[] =
    "# comment line\n"
    "\n"
    "a shed \"OLD SHED\" 1.5 planks 10,20,30 40,50,60 3,3 S shed_room\n";

WorldMap smallWorld() {
  WorldMap map;
  std::ostringstream warn;
  parseTownMap(kMap, map);
  parseBuildings(kBuildings, map, warn);
  return map;
}

}  // namespace

TEST(WorldMapTest, ParsesTilesAndStart) {
  WorldMap map = smallWorld();
  EXPECT_EQ(7, map.width());
  EXPECT_EQ(6, map.height());
  EXPECT_DOUBLE_EQ(3.5, map.start().x);
  EXPECT_DOUBLE_EQ(3.5, map.start().y);
  EXPECT_EQ('#', map.tile(-1, 0));
  EXPECT_EQ('#', map.tile(0, 99));
}

TEST(WorldMapTest, Ground) {
  WorldMap map = smallWorld();
  EXPECT_EQ(Ground::Street, map.groundAt(1.5, 3.5));
  EXPECT_EQ(Ground::Grass, map.groundAt(1.5, 4.5));
  EXPECT_EQ(Ground::Salt, map.groundAt(2.5, 4.5));
  EXPECT_EQ(Ground::Boardwalk, map.groundAt(4.5, 4.5));
  EXPECT_EQ(Ground::Sand, map.groundAt(5.5, 4.5));
  EXPECT_EQ(Ground::Sand, map.groundAt(3.5, 3.5));  // start marker
}

TEST(WorldMapTest, Blocking) {
  WorldMap map = smallWorld();
  EXPECT_TRUE(map.isBlocked(0.5, 0.5));   // rock
  EXPECT_TRUE(map.isBlocked(3.5, 1.5));   // building
  EXPECT_TRUE(map.isBlocked(3.5, 4.5));   // fence
  EXPECT_FALSE(map.isBlocked(5.5, 1.5));  // sand
  EXPECT_TRUE(map.isBlocked(-1.0, 1.0));  // outside the map
  ASSERT_NE(nullptr, map.buildingAt(4, 2));
  EXPECT_EQ("shed", map.buildingAt(4, 2)->id);
  EXPECT_EQ(nullptr, map.buildingAt(5, 2));
}

TEST(WorldMapTest, UnknownCharIsOpenSand) {
  WorldMap map;
  parseTownMap("..\n.?\n", map);
  EXPECT_FALSE(map.isBlocked(1.5, 1.5));
  EXPECT_EQ(Ground::Sand, map.groundAt(1.5, 1.5));
  EXPECT_FALSE(map.isBlocked(0.5, 0.5));
}

TEST(WorldMapTest, UndefinedBuildingLetterIsOpen) {
  WorldMap map;
  parseTownMap("q.\n..\n", map);
  EXPECT_FALSE(map.isBlocked(0.5, 0.5));
}

TEST(WorldMapTest, RaggedRowsPadded) {
  WorldMap map;
  ASSERT_TRUE(parseTownMap("....\n..\r\n.\n", map));
  EXPECT_EQ(4, map.width());
  EXPECT_EQ(3, map.height());
  EXPECT_FALSE(map.isBlocked(3.5, 2.5));
}

TEST(WorldMapTest, EmptyMapRejected) {
  WorldMap map;
  EXPECT_FALSE(parseTownMap("", map));
  EXPECT_FALSE(parseTownMap("\n\n", map));
}

TEST(WorldMapTest, NoStartUsesFirstOpenTile) {
  WorldMap map;
  parseTownMap("##\n#.\n", map);
  EXPECT_DOUBLE_EQ(1.5, map.start().x);
  EXPECT_DOUBLE_EQ(1.5, map.start().y);
}

TEST(WorldMapTest, DoorNear) {
  WorldMap map = smallWorld();
  ASSERT_NE(nullptr, map.doorNear(3.5, 3.5, 1.5));
  EXPECT_EQ("shed_room", map.doorNear(3.5, 3.5, 1.5)->roomId);
  EXPECT_EQ(nullptr, map.doorNear(5.5, 4.5, 1.5));
}

TEST(WorldMapTest, ZoneAt) {
  WorldMap map = smallWorld();
  std::ostringstream warn;
  EXPECT_EQ(2, parseZones("inner 1 1 2 2 The inner part.\n"
                          "outer 0 0 6 5 Everything else.\n",
                          map, warn));
  ASSERT_NE(nullptr, map.zoneAt(1.5, 1.5));
  EXPECT_EQ("inner", map.zoneAt(1.5, 1.5)->roomId);
  EXPECT_EQ("The inner part.", map.zoneAt(1.5, 1.5)->description);
  EXPECT_EQ("outer", map.zoneAt(5.5, 4.5)->roomId);
  EXPECT_EQ(nullptr, map.zoneAt(40.0, 40.0));
}
