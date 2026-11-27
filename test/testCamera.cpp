#include <gtest/gtest.h>

#include <cmath>
#include <random>

#include "Camera.hpp"
#include "WorldFiles.hpp"
#include "WorldMap.hpp"

namespace {

const double kPi = 3.14159265358979323846;

WorldMap room() {
  WorldMap map;
  parseTownMap(
      "##########\n"
      "#........#\n"
      "#........#\n"
      "#...##...#\n"
      "#........#\n"
      "##########\n",
      map);
  return map;
}

}  // namespace

TEST(CameraTest, TurnWraps) {
  Camera c(1.5, 1.5, 0.0);
  c.turn(-0.1);
  EXPECT_NEAR(2 * kPi - 0.1, c.angle(), 1e-9);
  c.turn(0.2);
  EXPECT_NEAR(0.1, c.angle(), 1e-9);
  EXPECT_NEAR(std::cos(0.1), c.dirX(), 1e-9);
  EXPECT_NEAR(std::sin(0.1), c.dirY(), 1e-9);
}

TEST(CameraTest, PlaneIsPerpendicular) {
  Camera c(1.5, 1.5, kPi);  // facing west
  EXPECT_NEAR(-1.0, c.dirX(), 1e-9);
  EXPECT_NEAR(0.0, c.dirX() * c.planeX() + c.dirY() * c.planeY(), 1e-9);
  EXPECT_GT(std::hypot(c.planeX(), c.planeY()), 0.5);
}

TEST(CameraTest, StepForwardAndBack) {
  WorldMap map = room();
  Camera c(2.5, 1.5, 0.0);  // facing east
  EXPECT_TRUE(c.step(map, 0.5));
  EXPECT_NEAR(3.0, c.x(), 1e-9);
  EXPECT_TRUE(c.step(map, -0.5));
  EXPECT_NEAR(2.5, c.x(), 1e-9);
}

TEST(CameraTest, BlockedByWall) {
  WorldMap map = room();
  Camera c(1.3, 1.5, kPi);  // facing the west wall, 0.3 away
  EXPECT_FALSE(c.step(map, 0.5));
  EXPECT_NEAR(1.3, c.x(), 1e-9);
}

TEST(CameraTest, SlideAlongWall) {
  WorldMap map = room();
  Camera c(3.0, 1.3, -kPi / 4);  // heading north-east into the north wall
  EXPECT_TRUE(c.step(map, 0.5));
  EXPECT_GT(c.x(), 3.2);
  EXPECT_NEAR(1.3, c.y(), 1e-9);
}

TEST(CameraTest, NeverInsideWall) {
  WorldMap map = room();
  Camera c(2.5, 2.5, 0.0);
  std::mt19937 rng(7);
  std::uniform_real_distribution<double> turn(-1.0, 1.0);
  for (int i = 0; i < 500; ++i) {
    c.turn(turn(rng));
    c.step(map, (i % 3 == 0) ? -0.5 : 1.0);
    ASSERT_FALSE(map.isBlocked(c.x(), c.y())) << "step " << i;
  }
}

TEST(CameraTest, SetPosition) {
  Camera c(0.5, 0.5, 0.0);
  c.setPosition(4.5, 2.5);
  c.setAngle(kPi);
  EXPECT_DOUBLE_EQ(4.5, c.x());
  EXPECT_DOUBLE_EQ(2.5, c.y());
  EXPECT_NEAR(kPi, c.angle(), 1e-9);
}
