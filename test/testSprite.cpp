#include <gtest/gtest.h>

#include <sstream>
#include <string>

#include "Rgb.hpp"
#include "Sprite.hpp"
#include "WorldFiles.hpp"
#include "WorldMap.hpp"

TEST(SpriteTest, ParsesSizePaletteAndRows) {
  Sprite s;
  std::string why;
  ASSERT_TRUE(parseSprite("# a comment\n"
                          "size 1.5 0.5\n"
                          "A = 10 20 30\n"
                          "B=1 2 3\n"
                          "A.B\n"
                          "AA\n",
                          s, why))
      << why;
  EXPECT_DOUBLE_EQ(1.5, s.width);
  EXPECT_DOUBLE_EQ(0.5, s.height);
  EXPECT_EQ(3, s.columns());
  EXPECT_EQ(2, s.rowCount());
  Rgb c;
  ASSERT_TRUE(s.pixel(0, 0, c));
  EXPECT_TRUE(c == Rgb(10, 20, 30));
  EXPECT_FALSE(s.pixel(1, 0, c));  // '.' is see-through
  ASSERT_TRUE(s.pixel(2, 0, c));
  EXPECT_TRUE(c == Rgb(1, 2, 3));
  EXPECT_FALSE(s.pixel(2, 1, c));  // short row padded with '.'
  EXPECT_FALSE(s.pixel(-1, 0, c));
  EXPECT_FALSE(s.pixel(0, 9, c));
}

TEST(SpriteTest, UnknownLetterIsSeeThrough) {
  Sprite s;
  std::string why;
  ASSERT_TRUE(parseSprite("size 1 1\nA = 1 1 1\nAZ\n", s, why));
  Rgb c;
  EXPECT_FALSE(s.pixel(1, 0, c));
}

TEST(SpriteTest, RejectsBadSprites) {
  Sprite s;
  std::string why;
  EXPECT_FALSE(parseSprite("A = 1 1 1\nAA\n", s, why));  // no size
  EXPECT_FALSE(parseSprite("size big 1\nA = 1 1 1\nAA\n", s, why));
  EXPECT_FALSE(parseSprite("size 1 1\nA = 1 x 1\nAA\n", s, why));
  EXPECT_FALSE(parseSprite("size 1 1\nA = 1 1 1\n", s, why));  // no rows
  EXPECT_FALSE(why.empty());
}

TEST(PropsTest, ParsesProps) {
  WorldMap map;
  std::ostringstream warn;
  EXPECT_EQ(2, parseProps("# comment\ncactus 3.5 4.25 yes\nbarrel 1 2 no\n",
                          map, warn));
  ASSERT_EQ(2u, map.props().size());
  EXPECT_EQ("cactus", map.props()[0].sprite);
  EXPECT_DOUBLE_EQ(3.5, map.props()[0].pos.x);
  EXPECT_DOUBLE_EQ(4.25, map.props()[0].pos.y);
  EXPECT_TRUE(map.props()[0].blocking);
  EXPECT_FALSE(map.props()[1].blocking);
}

TEST(PropsTest, MalformedPropsSkipped) {
  WorldMap map;
  std::ostringstream warn;
  EXPECT_EQ(0, parseProps("cactus x 1 yes\ncactus 1 1 maybe\ncactus 1\n", map,
                          warn));
  EXPECT_NE(std::string::npos, warn.str().find("line 3"));
}

TEST(PropsTest, BlockingPropBlocks) {
  WorldMap map;
  std::ostringstream warn;
  parseTownMap("......\n......\n......\n", map);
  parseProps("barrel 2.5 1.5 yes\nbush 4.5 1.5 no\n", map, warn);
  EXPECT_TRUE(map.isBlocked(2.6, 1.5));
  EXPECT_FALSE(map.isBlocked(3.4, 1.5));
  EXPECT_FALSE(map.isBlocked(4.5, 1.5));
}

TEST(PropsTest, ShippedPropsHaveSprites) {
  std::ostringstream warn;
  WorldMap map = loadWorld("data/world", warn);
  EXPECT_EQ("", warn.str());
  EXPECT_GT(map.props().size(), 10u);
  for (const Prop& p : map.props()) {
    EXPECT_NE(nullptr, map.sprite(p.sprite)) << p.sprite;
  }
  EXPECT_NE(nullptr, map.sprite("horse"));
}
