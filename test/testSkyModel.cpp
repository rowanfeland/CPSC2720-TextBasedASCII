#include <gtest/gtest.h>

#include <sstream>
#include <string>

#include "Rgb.hpp"
#include "SkyModel.hpp"
#include "SkyState.hpp"

TEST(SkyModelTest, ParsesKeyframes) {
  SkyModel sky;
  std::ostringstream warn;
  EXPECT_EQ(2, sky.parse("# comment\n"
                         "06:00 10,20,30 40,50,60 0.5\n"
                         "18:00 110,120,130 140,150,160 1.0\n",
                         warn));
  EXPECT_EQ("", warn.str());
  EXPECT_EQ(2u, sky.size());
}

TEST(SkyModelTest, InterpolatesBetweenKeyframes) {
  SkyModel sky;
  std::ostringstream warn;
  sky.parse("06:00 0,0,0 0,0,0 0.0\n12:00 200,100,50 100,100,100 1.0\n", warn);
  SkyState s = sky.sample(9 * 60);
  EXPECT_TRUE(s.top == Rgb(100, 50, 25));
  EXPECT_NEAR(0.5, s.light, 1e-9);
}

TEST(SkyModelTest, WrapsAroundMidnight) {
  SkyModel sky;
  std::ostringstream warn;
  sky.parse("20:00 0,0,100 0,0,0 0.2\n04:00 0,0,200 0,0,0 0.6\n", warn);
  // Midnight is halfway from 20:00 to 04:00 the next day.
  EXPECT_NEAR(0.4, sky.sample(0).light, 1e-9);
  EXPECT_TRUE(sky.sample(0).top == Rgb(0, 0, 150));
}

TEST(SkyModelTest, MalformedLinesSkipped) {
  SkyModel sky;
  std::ostringstream warn;
  EXPECT_EQ(1, sky.parse("25:00 1,1,1 1,1,1 1\n"
                         "06:00 1,1 1,1,1 1\n"
                         "07:00 1,1,1 1,1,1 bright\n"
                         "08:00 1,1,1 1,1,1 0.9\n",
                         warn));
  EXPECT_NE(std::string::npos, warn.str().find("line 1"));
  EXPECT_NE(std::string::npos, warn.str().find("line 3"));
}

TEST(SkyModelTest, DefaultsWhenEmpty) {
  SkyModel sky;
  EXPECT_GT(sky.size(), 3u);
  SkyState noon = sky.sample(12 * 60);
  SkyState midnight = sky.sample(0);
  EXPECT_GT(noon.light, midnight.light);
}

TEST(SkyModelTest, SunByDayMoonAndStarsByNight) {
  SkyModel sky;
  SkyState noon = sky.sample(12 * 60);
  EXPECT_TRUE(noon.sunVisible);
  EXPECT_FALSE(noon.moon);
  EXPECT_GT(noon.sunElevation, 0.7);
  EXPECT_DOUBLE_EQ(0.0, noon.stars);
  SkyState sunset = sky.sample(17 * 60 + 30);
  EXPECT_FALSE(sunset.moon);
  EXPECT_GT(sunset.sunAzimuth, 2.5);  // low in the west
  EXPECT_LT(sunset.sunElevation, 0.5);
  SkyState night = sky.sample(1 * 60);
  EXPECT_TRUE(night.moon);
  EXPECT_GT(night.stars, 0.5);
}

TEST(SkyModelTest, LoadsShippedSky) {
  SkyModel sky;
  std::ostringstream warn;
  EXPECT_TRUE(sky.load("data/world/sky.txt", warn));
  EXPECT_EQ("", warn.str());
  EXPECT_FALSE(sky.load("data/world/missing-sky.txt", warn));
}
