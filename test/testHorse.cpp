#include <gtest/gtest.h>

#include <string>

#include "Horse.hpp"
#include "WorldFiles.hpp"
#include "WorldMap.hpp"

namespace {

WorldMap openMap() {
  WorldMap map;
  parseTownMap("........\n........\n........\n........\n", map);
  return map;
}

}  // namespace

TEST(HorseTest, StartsMountedWithRider) {
  Horse h(Point2(2.5, 2.5), true);
  EXPECT_TRUE(h.isMounted());
  EXPECT_DOUBLE_EQ(2.0, h.speedFactor());
  EXPECT_DOUBLE_EQ(kRidingEyeHeight, h.eyeHeight());
}

TEST(HorseTest, DismountLeavesHorseBeside) {
  WorldMap map = openMap();
  Horse h(Point2(2.5, 2.5), true);
  EXPECT_EQ("You swing down from the saddle.", h.dismount(map, Point2(3.5, 1.5), 0.0));
  EXPECT_FALSE(h.isMounted());
  EXPECT_DOUBLE_EQ(1.0, h.speedFactor());
  EXPECT_DOUBLE_EQ(kWalkingEyeHeight, h.eyeHeight());
  // Facing east, the horse is left to your right side (south).
  EXPECT_NEAR(3.5, h.position().x, 1e-9);
  EXPECT_GT(h.position().y, 1.5);
  EXPECT_EQ("You aren't riding.", h.dismount(map, Point2(3.5, 1.5), 0.0));
}

TEST(HorseTest, DismountAgainstWallStaysUnderYou) {
  WorldMap map;
  parseTownMap("....\n....\n####\n", map);
  Horse h(Point2(1.5, 1.5), true);
  h.dismount(map, Point2(1.5, 1.5), 0.0);
  EXPECT_NEAR(1.5, h.position().x, 1e-9);
  EXPECT_NEAR(1.5, h.position().y, 1e-9);
}

TEST(HorseTest, MountNeedsToBeClose) {
  Horse h(Point2(5.5, 1.5), false);
  EXPECT_EQ("Your horse is too far away.", h.mount(Point2(1.5, 1.5)));
  EXPECT_FALSE(h.isMounted());
  EXPECT_EQ("You swing up into the saddle.", h.mount(Point2(4.5, 1.5)));
  EXPECT_TRUE(h.isMounted());
  EXPECT_EQ("You're already riding.", h.mount(Point2(4.5, 1.5)));
}

TEST(HorseTest, FollowsRiderWhileMounted) {
  Horse h(Point2(1.5, 1.5), true);
  h.follow(Point2(3.0, 2.0));
  EXPECT_DOUBLE_EQ(3.0, h.position().x);
  Horse idle(Point2(1.5, 1.5), false);
  idle.follow(Point2(3.0, 2.0));
  EXPECT_DOUBLE_EQ(1.5, idle.position().x);
}
