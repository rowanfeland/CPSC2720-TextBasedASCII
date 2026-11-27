#include <gtest/gtest.h>

#include <string>

#include "Pgm.hpp"

TEST(PgmTest, ParsesP2WithComment) {
  GreyImage img(1, 1);
  ASSERT_TRUE(parsePgm("P2\n# made in GIMP\n3 1\n255\n0 128 255\n", img));
  EXPECT_EQ(3, img.width());
  EXPECT_EQ(1, img.height());
  EXPECT_EQ(0, img.get(0, 0));
  EXPECT_EQ(128, img.get(1, 0));
  EXPECT_EQ(255, img.get(2, 0));
}

TEST(PgmTest, ScalesMaxval) {
  GreyImage img(1, 1);
  ASSERT_TRUE(parsePgm("P2 2 1 15 0 15", img));
  EXPECT_EQ(0, img.get(0, 0));
  EXPECT_EQ(255, img.get(1, 0));
}

TEST(PgmTest, ParsesP5) {
  std::string data = "P5\n2 1\n255\n";
  data += static_cast<char>(10);
  data += static_cast<char>(200);
  GreyImage img(1, 1);
  ASSERT_TRUE(parsePgm(data, img));
  EXPECT_EQ(10, img.get(0, 0));
  EXPECT_EQ(200, img.get(1, 0));
}

TEST(PgmTest, RejectsMalformed) {
  GreyImage img(1, 1);
  EXPECT_FALSE(parsePgm("", img));
  EXPECT_FALSE(parsePgm("P7\n1 1\n255\n0\n", img));
  EXPECT_FALSE(parsePgm("P2 0 4 255", img));
  EXPECT_FALSE(parsePgm("P2 2 2 0 1 1 1 1", img));
  EXPECT_FALSE(parsePgm("P2 x 2 255", img));
}

TEST(PgmTest, RejectsTruncated) {
  GreyImage img(1, 1);
  EXPECT_FALSE(parsePgm("P2\n2 2\n255\n1 2 3\n", img));
  EXPECT_FALSE(parsePgm("P5\n2 2\n255\nab", img));
}

TEST(PgmTest, LoadsFile) {
  GreyImage img(1, 1);
  ASSERT_TRUE(loadPgm("test/data/art/tiny.pgm", img));
  EXPECT_EQ(2, img.width());
  EXPECT_EQ(85, img.get(1, 0));
  EXPECT_EQ(170, img.get(0, 1));
}

TEST(PgmTest, MissingFile) {
  GreyImage img(1, 1);
  EXPECT_FALSE(loadPgm("test/data/art/nope.pgm", img));
}
