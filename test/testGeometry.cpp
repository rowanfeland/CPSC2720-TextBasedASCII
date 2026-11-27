#include <gtest/gtest.h>
#include "Geometry.hpp"

TEST(GeometryTest, RectContains) {
  Rect r(2, 2, 3, 2);
  EXPECT_TRUE(r.contains(Point(2, 2)));
  EXPECT_TRUE(r.contains(Point(4, 3)));
  EXPECT_FALSE(r.contains(Point(5, 3)));
  EXPECT_FALSE(r.contains(Point(2, 4)));
  EXPECT_FALSE(r.contains(Point(1, 2)));
}

TEST(GeometryTest, PointEquality) {
  EXPECT_TRUE(Point(1, 2) == Point(1, 2));
  EXPECT_FALSE(Point(1, 2) == Point(2, 1));
  EXPECT_TRUE(Point(1, 2) != Point(2, 1));
}

TEST(GeometryTest, RectIntersects) {
  Rect a(0, 0, 3, 3);
  EXPECT_TRUE(a.intersects(Rect(2, 2, 3, 3)));
  EXPECT_FALSE(a.intersects(Rect(3, 0, 2, 2)));
  EXPECT_FALSE(a.intersects(Rect(0, 3, 2, 2)));
}
