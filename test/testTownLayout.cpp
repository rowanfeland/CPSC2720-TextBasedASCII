#include <gtest/gtest.h>

#include <set>
#include <string>
#include <vector>

#include "TownLayout.hpp"

TEST(TownLayoutTest, HasNineBuildings) {
  SceneMap map = TownLayout::buildMainStreet();
  EXPECT_EQ("Main Street", map.name());
  ASSERT_EQ(9u, map.buildings().size());
  std::set<std::string> ids;
  for (const Building& b : map.buildings()) {
    ids.insert(b.id);
  }
  const char* expected[] = {"hotel",  "saloon",        "gunsmith",
                            "sheriff", "stable",       "church",
                            "general_store", "telegraph", "graves"};
  for (const char* id : expected) {
    EXPECT_EQ(1u, ids.count(id)) << id;
  }
}

TEST(TownLayoutTest, NoOverlaps) {
  SceneMap map = TownLayout::buildMainStreet();
  const std::vector<Building>& b = map.buildings();
  for (size_t i = 0; i < b.size(); ++i) {
    for (size_t j = i + 1; j < b.size(); ++j) {
      EXPECT_FALSE(b[i].footprint.intersects(b[j].footprint))
          << b[i].id << " overlaps " << b[j].id;
    }
  }
}

TEST(TownLayoutTest, BuildingsInsideMap) {
  SceneMap map = TownLayout::buildMainStreet();
  for (const Building& b : map.buildings()) {
    const Rect& r = b.footprint;
    EXPECT_TRUE(map.isInside(Point(r.x, r.y))) << b.id;
    EXPECT_TRUE(map.isInside(Point(r.x + r.width - 1, r.y + r.height - 1)))
        << b.id;
  }
}

TEST(TownLayoutTest, DoorsOnEdgeAndReachable) {
  SceneMap map = TownLayout::buildMainStreet();
  for (const Building& b : map.buildings()) {
    const Rect& r = b.footprint;
    EXPECT_TRUE(r.contains(b.door)) << b.id;
    const bool onTop = b.door.y == r.y;
    const bool onBottom = b.door.y == r.y + r.height - 1;
    EXPECT_TRUE(onTop || onBottom) << b.id;
    EXPECT_TRUE(map.isWalkable(b.door)) << b.id;
    // The cell just outside the door, toward the road, must be walkable.
    const Point outside(b.door.x, onBottom ? b.door.y + 1 : b.door.y - 1);
    EXPECT_TRUE(map.isWalkable(outside)) << b.id;
  }
}

TEST(TownLayoutTest, LabelsFitInsideBoxes) {
  SceneMap map = TownLayout::buildMainStreet();
  for (const Building& b : map.buildings()) {
    EXPECT_LE(static_cast<int>(b.label.size()), b.footprint.width - 2) << b.id;
  }
}

TEST(TownLayoutTest, StartWalkableOnRoadAtEastEdge) {
  SceneMap map = TownLayout::buildMainStreet();
  const Point start = TownLayout::startPosition();
  EXPECT_TRUE(map.isWalkable(start));
  EXPECT_TRUE(map.isRoad(start.y));
  EXPECT_EQ(map.width() - 2, start.x);
}
