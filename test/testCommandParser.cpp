#include <gtest/gtest.h>

#include <string>

#include "CommandParser.hpp"
#include "CommandRegistry.hpp"
#include "Command.hpp"

TEST(CommandParserTest, ParserNormalizes) {
  ParsedCommand c = CommandParser::parse("  TAKE   Tin Cup ");
  EXPECT_EQ("take", c.verb);
  EXPECT_EQ("tin cup", c.noun);
  EXPECT_EQ("", c.target);
  EXPECT_FALSE(c.isEmpty());
}

TEST(CommandParserTest, ParserTarget) {
  ParsedCommand use = CommandParser::parse("use key on door");
  EXPECT_EQ("use", use.verb);
  EXPECT_EQ("key", use.noun);
  EXPECT_EQ("door", use.target);

  ParsedCommand give = CommandParser::parse("give apple to horse");
  EXPECT_EQ("give", give.verb);
  EXPECT_EQ("apple", give.noun);
  EXPECT_EQ("horse", give.target);
}

TEST(CommandParserTest, ParserEmpty) {
  EXPECT_TRUE(CommandParser::parse("   ").isEmpty());
  EXPECT_TRUE(CommandParser::parse("").isEmpty());
  EXPECT_FALSE(CommandParser::parse("look").isEmpty());
}

TEST(CommandRegistryTest, ExpandAliases) {
  CommandRegistry reg;
  CommandRegistry::registerDefaults(reg);
  EXPECT_EQ("go north", reg.expand("n"));
  EXPECT_EQ("go saloon", reg.expand("enter saloon"));
  EXPECT_EQ("go out", reg.expand("leave"));
  EXPECT_EQ("inventory", reg.expand("i"));
  EXPECT_EQ("look", reg.expand("look"));
  EXPECT_EQ("quit", reg.expand("q"));
}

TEST(CommandRegistryTest, FindUnknownNull) {
  CommandRegistry reg;
  CommandRegistry::registerDefaults(reg);
  EXPECT_EQ(nullptr, reg.find("dance"));
  EXPECT_NE(nullptr, reg.find("look"));
}

TEST(CommandRegistryTest, HelpListsAll) {
  CommandRegistry reg;
  CommandRegistry::registerDefaults(reg);
  const std::string help = reg.helpText();
  int lines = 0;
  for (std::string::size_type i = 0; i < help.size(); ++i) {
    if (help[i] == '\n') {
      ++lines;
    }
  }
  EXPECT_EQ(7, lines);
  EXPECT_NE(std::string::npos, help.find("go"));
  EXPECT_NE(std::string::npos, help.find("quit"));
}
