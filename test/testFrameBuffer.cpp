#include <gtest/gtest.h>

#include "FrameBuffer.hpp"
#include "Rgb.hpp"

TEST(FrameBufferTest, SetGetClip) {
  FrameBuffer fb(3, 2);
  EXPECT_EQ(3, fb.width());
  EXPECT_EQ(2, fb.height());
  EXPECT_TRUE(fb.get(1, 1) == Rgb(0, 0, 0));
  fb.set(1, 1, Rgb(10, 20, 30));
  EXPECT_TRUE(fb.get(1, 1) == Rgb(10, 20, 30));
  fb.set(-1, 0, Rgb(1, 1, 1));
  fb.set(3, 0, Rgb(1, 1, 1));
  EXPECT_TRUE(fb.get(9, 9) == Rgb(0, 0, 0));
  fb.fill(Rgb(5, 5, 5));
  EXPECT_TRUE(fb.get(2, 0) == Rgb(5, 5, 5));
}

TEST(FrameBufferTest, NegativeSizeIsEmpty) {
  FrameBuffer fb(-3, -1);
  EXPECT_EQ(0, fb.width());
  EXPECT_EQ(0, fb.height());
}

TEST(RgbTest, MixScale) {
  EXPECT_TRUE(mix(Rgb(0, 0, 0), Rgb(200, 100, 50), 0.5) == Rgb(100, 50, 25));
  EXPECT_TRUE(mix(Rgb(0, 0, 0), Rgb(200, 100, 50), 2.0) == Rgb(200, 100, 50));
  EXPECT_TRUE(scale(Rgb(100, 200, 50), 2.0) == Rgb(200, 255, 100));
  EXPECT_TRUE(scale(Rgb(100, 200, 50), -1.0) == Rgb(0, 0, 0));
  EXPECT_FALSE(Rgb(1, 2, 3) == Rgb(1, 2, 4));
  EXPECT_TRUE(Rgb(1, 2, 3) != Rgb(1, 2, 4));
}
