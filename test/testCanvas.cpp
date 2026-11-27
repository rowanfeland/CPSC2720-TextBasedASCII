#include <gtest/gtest.h>
#include "Canvas.hpp"

TEST(CanvasTest, StartsFilled) {
  Canvas c(3, 2, '.');
  EXPECT_EQ(3, c.width());
  EXPECT_EQ(2, c.height());
  EXPECT_EQ("...\n...\n", c.toString());
}

TEST(CanvasTest, PutGet) {
  Canvas c(3, 2);
  c.put(1, 1, '@');
  EXPECT_EQ('@', c.get(1, 1));
  EXPECT_EQ(' ', c.get(0, 0));
}

TEST(CanvasTest, ClipsOutOfBounds) {
  Canvas c(10, 1);
  c.put(-1, 0, 'x');
  c.put(10, 0, 'x');
  c.put(0, 5, 'x');
  EXPECT_EQ(' ', c.get(-1, 0));
  EXPECT_EQ(' ', c.get(0, 9));
  c.drawText(8, 0, "hello");
  EXPECT_EQ("        he\n", c.toString());
  c.drawText(-2, 0, "abc");
  EXPECT_EQ("c       he\n", c.toString());
}

TEST(CanvasTest, DrawBox) {
  Canvas c(3, 3);
  c.drawBox(Rect(0, 0, 3, 3));
  EXPECT_EQ("+-+\n| |\n+-+\n", c.toString());
}

TEST(CanvasTest, DrawBoxPartlyOffCanvas) {
  Canvas c(3, 2);
  c.drawBox(Rect(1, 0, 4, 4));
  EXPECT_EQ(" +-\n | \n", c.toString());
}

TEST(CanvasTest, BlitTransparent) {
  Canvas c(3, 2, 'x');
  c.blit(0, 0, "a b\n c");
  EXPECT_EQ("axb\nxcx\n", c.toString());
}

TEST(CanvasTest, NegativeSizeIsEmpty) {
  Canvas c(-2, -1);
  EXPECT_EQ(0, c.width());
  EXPECT_EQ(0, c.height());
  EXPECT_EQ("", c.toString());
}
