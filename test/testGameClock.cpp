#include <gtest/gtest.h>

#include "GameClock.hpp"

TEST(GameClockTest, StartsAtSunset) {
  GameClock clock;
  EXPECT_EQ(17 * 60 + 30, clock.minutes());
  EXPECT_EQ("17:30", clock.format());
}

TEST(GameClockTest, AdvanceAndFormat) {
  GameClock clock(9 * 60 + 5);
  EXPECT_EQ("09:05", clock.format());
  EXPECT_EQ(0, clock.advance(10));
  EXPECT_EQ("09:15", clock.format());
}

TEST(GameClockTest, WrapsPastMidnight) {
  GameClock clock(23 * 60 + 50);
  EXPECT_EQ(1, clock.advance(20));
  EXPECT_EQ("00:10", clock.format());
  EXPECT_EQ(2, clock.advance(2 * 24 * 60));
  EXPECT_EQ("00:10", clock.format());
}

TEST(GameClockTest, NegativeIgnored) {
  GameClock clock(60);
  EXPECT_EQ(0, clock.advance(-30));
  EXPECT_EQ(60, clock.minutes());
}

TEST(GameClockTest, StartWrapped) {
  GameClock clock(24 * 60 + 15);
  EXPECT_EQ("00:15", clock.format());
}
