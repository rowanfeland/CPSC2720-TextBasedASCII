#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "TextUtil.hpp"

TEST(TextUtilTest, NormalizeTrimsCaseSpaces) {
  EXPECT_EQ("enter saloon", normalize("  ENTER   Saloon "));
  EXPECT_EQ("", normalize(""));
  EXPECT_EQ("", normalize("   \t "));
  EXPECT_EQ("quit", normalize("Quit\r"));
}

TEST(TextUtilTest, JoinList) {
  EXPECT_EQ("", joinList(std::vector<std::string>()));
  EXPECT_EQ("a", joinList({"a"}));
  EXPECT_EQ("a and b", joinList({"a", "b"}));
  EXPECT_EQ("a, b and c", joinList({"a", "b", "c"}));
}
