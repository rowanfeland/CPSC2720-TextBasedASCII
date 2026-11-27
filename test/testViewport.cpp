#include <gtest/gtest.h>
#include "Viewport.hpp"

TEST(ViewportTest, Size) {
  Viewport vp(10, 4);
  EXPECT_EQ(10, vp.width());
  EXPECT_EQ(4, vp.height());
  EXPECT_TRUE(vp.origin() == Point(0, 0));
}

TEST(ViewportTest, FollowCentres) {
  Viewport vp(10, 4);
  vp.follow(Point(50, 10), 100, 20);
  EXPECT_TRUE(vp.origin() == Point(45, 8));
}

TEST(ViewportTest, FollowClampsLow) {
  Viewport vp(10, 4);
  vp.follow(Point(0, 0), 100, 20);
  EXPECT_TRUE(vp.origin() == Point(0, 0));
}

TEST(ViewportTest, FollowClampsHigh) {
  Viewport vp(10, 4);
  vp.follow(Point(99, 19), 100, 20);
  EXPECT_TRUE(vp.origin() == Point(90, 16));
}

TEST(ViewportTest, FollowSmallMap) {
  Viewport vp(10, 4);
  vp.follow(Point(4, 1), 5, 2);
  EXPECT_TRUE(vp.origin() == Point(0, 0));
}

TEST(ViewportTest, ToScreen) {
  Viewport vp(10, 4);
  vp.follow(Point(50, 10), 100, 20);
  EXPECT_TRUE(vp.toScreen(Point(50, 10)) == Point(5, 2));
}
