#include <gtest/gtest.h>
#include "AsciiArtLibrary.hpp"

TEST(AsciiArtLibraryTest, LoadsDirectory) {
  AsciiArtLibrary lib;
  ASSERT_TRUE(lib.loadFromDirectory("test/data/art"));
  EXPECT_TRUE(lib.hasArt("hotel"));
  EXPECT_EQ("HOTEL\n[] []\n", lib.getArt("hotel"));
}

TEST(AsciiArtLibraryTest, IgnoresNonTxtFiles) {
  AsciiArtLibrary lib;
  ASSERT_TRUE(lib.loadFromDirectory("test/data/art"));
  EXPECT_FALSE(lib.hasArt("notes"));
  EXPECT_FALSE(lib.hasArt("notes.md"));
}

TEST(AsciiArtLibraryTest, MissingDirectory) {
  AsciiArtLibrary lib;
  EXPECT_FALSE(lib.loadFromDirectory("test/data/does-not-exist"));
  EXPECT_FALSE(lib.hasArt("hotel"));
}

TEST(AsciiArtLibraryTest, UnknownKey) {
  AsciiArtLibrary lib;
  EXPECT_FALSE(lib.hasArt("saloon"));
  EXPECT_EQ("", lib.getArt("saloon"));
}

TEST(AsciiArtLibraryTest, AddArt) {
  AsciiArtLibrary lib;
  lib.addArt("well", "( )");
  EXPECT_TRUE(lib.hasArt("well"));
  EXPECT_EQ("( )", lib.getArt("well"));
}
