#include <gtest/gtest.h>
#include "GreyImage.hpp"

TEST(GreyImageTest, GetSetClamp) {
  GreyImage img(3, 2, 10);
  EXPECT_EQ(3, img.width());
  EXPECT_EQ(2, img.height());
  EXPECT_EQ(10, img.get(2, 1));
  img.set(0, 0, 300);
  EXPECT_EQ(255, img.get(0, 0));
  img.set(1, 0, -4);
  EXPECT_EQ(0, img.get(1, 0));
  img.set(9, 9, 50);
  EXPECT_EQ(0, img.get(9, 9));
  EXPECT_EQ(0, img.get(-1, 0));
}

TEST(GreyImageTest, NegativeSizeIsEmpty) {
  GreyImage img(-1, -5);
  EXPECT_EQ(0, img.width());
  EXPECT_EQ(0, img.height());
}

TEST(GreyImageTest, FillRect) {
  GreyImage img(4, 3);
  img.fillRect(Rect(1, 1, 2, 5), 99);
  EXPECT_EQ(0, img.get(0, 1));
  EXPECT_EQ(99, img.get(1, 1));
  EXPECT_EQ(99, img.get(2, 2));
  EXPECT_EQ(0, img.get(3, 2));
  EXPECT_EQ(0, img.get(1, 0));
}

TEST(GreyImageTest, HalvedAverages) {
  GreyImage img(2, 2);
  img.set(0, 0, 0);
  img.set(1, 0, 100);
  img.set(0, 1, 200);
  img.set(1, 1, 100);
  GreyImage half = img.halved();
  EXPECT_EQ(1, half.width());
  EXPECT_EQ(1, half.height());
  EXPECT_EQ(100, half.get(0, 0));
}
