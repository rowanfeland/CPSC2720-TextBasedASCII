#include <gtest/gtest.h>

#include <cmath>
#include <cstdlib>
#include <sstream>
#include <vector>

#include "Camera.hpp"
#include "FrameBuffer.hpp"
#include "Raycaster.hpp"
#include "Rgb.hpp"
#include "WallTextures.hpp"
#include "WorldFiles.hpp"
#include "WorldMap.hpp"

namespace {

const double kPi = 3.14159265358979323846;

WorldMap mapFrom(const char* tiles) {
  WorldMap map;
  std::ostringstream warn;
  parseTownMap(tiles, map);
  parseBuildings("a shed \"SHED\" 1.2 planks 150,90,60 230,220,190 3,4 S shed\n",
                 map, warn);
  return map;
}

SkyState daySky() {
  SkyState s;
  s.sunVisible = false;
  s.stars = 0;
  return s;
}

int distance(const Rgb& a, const Rgb& b) {
  return std::abs(a.r - b.r) + std::abs(a.g - b.g) + std::abs(a.b - b.b);
}

}  // namespace

TEST(RaycasterTest, SkyAboveHorizon) {
  WorldMap map = mapFrom("......\n......\n..@...\n......\n");
  RenderInput in(map, Camera(2.5, 2.5, 0.0));
  in.sky = daySky();
  FrameBuffer fb(80, 40);
  std::vector<double> depth;
  renderView(in, fb, depth);
  EXPECT_TRUE(fb.get(0, 0) == in.sky.top);
  EXPECT_TRUE(fb.get(40, 0) == in.sky.top);
}

TEST(RaycasterTest, OpenDesertNoWall) {
  WorldMap map = mapFrom("......\n......\n..@...\n......\n");
  RenderInput in(map, Camera(2.5, 2.5, 0.0));
  in.sky = daySky();
  FrameBuffer fb(40, 20);
  std::vector<double> depth;
  renderView(in, fb, depth);
  ASSERT_EQ(40u * 20u, depth.size());
  for (double d : depth) {
    EXPECT_GT(d, 1e8);
  }
}

TEST(RaycasterTest, WallAhead) {
  // Shed facade on row 3; camera on row 5 looking north at it.
  WorldMap map = mapFrom(
      "........\n"
      "..aaaa..\n"
      "..aaaa..\n"
      "..aaaa..\n"
      "........\n"
      "...@....\n");
  RenderInput in(map, Camera(3.5, 5.5, -kPi / 2));
  in.sky = daySky();
  FrameBuffer fb(80, 40);
  std::vector<double> depth;
  renderView(in, fb, depth);
  const int horizonRow = static_cast<int>(40 * kHorizonFraction);
  EXPECT_NEAR(1.5, depth[horizonRow * 80 + 40], 0.05);
  EXPECT_FALSE(fb.get(40, horizonRow) == in.sky.horizon);
}

TEST(RaycasterTest, FogFarther) {
  const char near[] =
      "..........................\n"
      "aaaaaaaaaaaaaaaaaaaaaaaaaa\n"
      "..........................\n"
      "..........................\n"
      "...........@..............\n";
  const char far[] =
      "..........................\n"
      "aaaaaaaaaaaaaaaaaaaaaaaaaa\n"
      "..........................\n"
      "..........................\n"
      "..........................\n"
      "..........................\n"
      "..........................\n"
      "..........................\n"
      "..........................\n"
      "..........................\n"
      "..........................\n"
      "..........................\n"
      "..........................\n"
      "..........................\n"
      "..........................\n"
      "..........................\n"
      "..........................\n"
      "..........................\n"
      "..........................\n"
      "..........................\n"
      "...........@..............\n";
  WorldMap a = mapFrom(near);
  WorldMap b = mapFrom(far);
  FrameBuffer fa(80, 40);
  FrameBuffer fbuf(80, 40);
  std::vector<double> depth;
  RenderInput ina(a, Camera(a.start().x, a.start().y, -kPi / 2));
  RenderInput inb(b, Camera(b.start().x, b.start().y, -kPi / 2));
  ina.sky = daySky();
  inb.sky = daySky();
  renderView(ina, fa, depth);
  renderView(inb, fbuf, depth);
  const int row = static_cast<int>(40 * kHorizonFraction);
  EXPECT_LT(distance(fbuf.get(40, row), inb.sky.horizon),
            distance(fa.get(40, row), ina.sky.horizon));
}

TEST(RaycasterTest, DeterministicFrame) {
  WorldMap map = mapFrom("........\n..aaaa..\n........\n...@....\n");
  RenderInput in(map, Camera(3.5, 3.5, -1.0));
  FrameBuffer one(60, 30);
  FrameBuffer two(60, 30);
  std::vector<double> depth;
  renderView(in, one, depth);
  renderView(in, two, depth);
  for (int y = 0; y < 30; ++y) {
    for (int x = 0; x < 60; ++x) {
      ASSERT_TRUE(one.get(x, y) == two.get(x, y));
    }
  }
}

TEST(RaycasterTest, DarkerAtLowLight) {
  WorldMap map = mapFrom("......\n......\n..@...\n......\n");
  RenderInput in(map, Camera(2.5, 2.5, 0.0));
  in.sky = daySky();
  FrameBuffer bright(40, 20);
  FrameBuffer dark(40, 20);
  std::vector<double> depth;
  renderView(in, bright, depth);
  in.sky.light = 0.2;
  renderView(in, dark, depth);
  const Rgb b = bright.get(20, 19);
  const Rgb d = dark.get(20, 19);
  EXPECT_LT(d.r + d.g + d.b, b.r + b.g + b.b);
}

TEST(RaycasterTest, SunDrawnWhenFacingIt) {
  WorldMap map = mapFrom("......\n......\n..@...\n......\n");
  RenderInput in(map, Camera(2.5, 2.5, kPi));
  in.sky = SkyState();  // sunset: sun low in the west
  FrameBuffer fb(80, 40);
  std::vector<double> depth;
  renderView(in, fb, depth);
  bool sunPixel = false;
  for (int y = 0; y < 20; ++y) {
    for (int x = 30; x < 50; ++x) {
      sunPixel = sunPixel || fb.get(x, y) == kSunColour;
    }
  }
  EXPECT_TRUE(sunPixel);
}

TEST(WallTexturesTest, DoorAndWindowOnFacade) {
  BuildingDef b;
  b.letter = 'a';
  b.id = "shed";
  b.style = "planks";
  b.height = 1.5;
  b.base = Rgb(150, 90, 60);
  b.trim = Rgb(230, 220, 190);
  b.doorX = 3;
  b.doorY = 4;
  b.facing = 'S';
  bool glows = false;
  const Rgb door = wallTexel(b, 3.5, 0.3, true, false, glows);
  const Rgb plank = wallTexel(b, 2.05, 0.3, true, false, glows);
  EXPECT_FALSE(door == plank);
  const Rgb dayWindow = wallTexel(b, 2.5, 0.6, true, false, glows);
  EXPECT_FALSE(glows);
  wallTexel(b, 2.5, 0.6, true, true, glows);
  (void)dayWindow;
  // At night at least some windows glow.
  bool anyGlow = false;
  for (int cell = 0; cell < 12; ++cell) {
    wallTexel(b, cell + 0.5, 0.6, true, true, glows);
    anyGlow = anyGlow || glows;
  }
  EXPECT_TRUE(anyGlow);
}

TEST(WallTexturesTest, EveryStyleProducesColour) {
  const char* styles[] = {"planks", "adobe", "barn", "church", "saloon",
                          "mine", "well", "rock", "fence", "unknown"};
  for (const char* style : styles) {
    BuildingDef b;
    b.letter = 'a';
    b.style = style;
    b.height = 1.5;
    b.base = Rgb(120, 100, 80);
    b.trim = Rgb(200, 190, 170);
    b.doorX = 0;
    b.doorY = 0;
    b.facing = 'E';
    bool glows = false;
    for (double v = 0.05; v < 1.5; v += 0.1) {
      wallTexel(b, 0.5, v, true, true, glows);
      wallTexel(b, 3.5, v, false, false, glows);
    }
  }
  SUCCEED();
}
