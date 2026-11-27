#include <gtest/gtest.h>

#include <sstream>
#include <string>

#include "WorldFiles.hpp"
#include "WorldMap.hpp"

TEST(WorldFilesTest, ParsesBuildingLine) {
  WorldMap map;
  std::ostringstream warn;
  parseTownMap("aa\n..\n", map);
  ASSERT_EQ(1, parseBuildings(
                   "a store \"GENERAL STORE\" 1.5 planks 104,132,94 "
                   "235,225,195 1,1 N general_store\n",
                   map, warn));
  const BuildingDef* b = map.buildingAt(0, 0);
  ASSERT_NE(nullptr, b);
  EXPECT_EQ('a', b->letter);
  EXPECT_EQ("store", b->id);
  EXPECT_EQ("GENERAL STORE", b->label);
  EXPECT_DOUBLE_EQ(1.5, b->height);
  EXPECT_EQ("planks", b->style);
  EXPECT_TRUE(b->base == Rgb(104, 132, 94));
  EXPECT_TRUE(b->trim == Rgb(235, 225, 195));
  EXPECT_EQ(1, b->doorX);
  EXPECT_EQ(1, b->doorY);
  EXPECT_EQ('N', b->facing);
  EXPECT_EQ("general_store", b->roomId);
  EXPECT_EQ("", warn.str());
}

TEST(WorldFilesTest, MalformedBuildingSkipped) {
  WorldMap map;
  std::ostringstream warn;
  const char text[] =
      "a shed \"SHED\" tall planks 1,2,3 4,5,6 1,1 S room\n"    // bad height
      "b shed \"SHED\" 1 planks 1,2 4,5,6 1,1 S room\n"         // bad colour
      "c shed \"SHED\" 1 planks 1,2,3 4,5,6 1 S room\n"         // bad door
      "d shed \"SHED\" 1 planks 1,2,3 4,5,6 1,1 Q room\n"       // bad facing
      "ee shed \"SHED\" 1 planks 1,2,3 4,5,6 1,1 S room\n"      // bad letter
      "f shed \"SHED 1 planks 1,2,3 4,5,6 1,1 S room\n"         // open quote
      "g shed\n";                                                // too short
  EXPECT_EQ(0, parseBuildings(text, map, warn));
  EXPECT_NE(std::string::npos, warn.str().find("line 1"));
  EXPECT_NE(std::string::npos, warn.str().find("line 7"));
}

TEST(WorldFilesTest, MalformedZoneSkipped) {
  WorldMap map;
  std::ostringstream warn;
  EXPECT_EQ(1, parseZones("good 0 0 1 1 Fine.\nbad x 0 1 1 Nope.\nshort 1 2\n",
                          map, warn));
  EXPECT_NE(std::string::npos, warn.str().find("line 2"));
  EXPECT_NE(std::string::npos, warn.str().find("line 3"));
}

TEST(WorldFilesTest, LoadsShippedWorld) {
  std::ostringstream warn;
  WorldMap map = loadWorld("data/world", warn);
  EXPECT_EQ("", warn.str());
  EXPECT_EQ(64, map.width());
  EXPECT_EQ(40, map.height());
  EXPECT_EQ(10u, map.buildings().size());
  EXPECT_EQ(5u, map.zones().size());
  EXPECT_FALSE(map.isBlocked(map.start().x, map.start().y));
  ASSERT_NE(nullptr, map.zoneAt(map.start().x, map.start().y));
  EXPECT_EQ("desert_road", map.zoneAt(map.start().x, map.start().y)->roomId);
  for (const BuildingDef& b : map.buildings()) {
    EXPECT_FALSE(map.isBlocked(b.doorX + 0.5, b.doorY + 0.5)) << b.id;
  }
}

TEST(WorldFilesTest, MissingDirFallsBack) {
  std::ostringstream warn;
  WorldMap map = loadWorld("test/data/no-such-world", warn);
  EXPECT_NE(std::string::npos, warn.str().find("test/data/no-such-world"));
  EXPECT_GT(map.width(), 0);
  EXPECT_FALSE(map.isBlocked(map.start().x, map.start().y));
  ASSERT_NE(nullptr, map.zoneAt(map.start().x, map.start().y));
}
