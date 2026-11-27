#include <gtest/gtest.h>

#include <sstream>

#include "ConsoleDisplay.hpp"
#include "ConsoleInput.hpp"

TEST(ConsoleDisplayTest, PromptNoNewline) {
  std::ostringstream out;
  ConsoleDisplay display(out);
  display.prompt("> ");
  EXPECT_EQ("> ", out.str());
}

TEST(ConsoleDisplayTest, ShowAddsNewline) {
  std::ostringstream out;
  ConsoleDisplay display(out);
  display.show("Howdy.");
  EXPECT_EQ("Howdy.\n", out.str());
}

TEST(ConsoleDisplayTest, ShowArtExactly) {
  std::ostringstream out;
  ConsoleDisplay display(out);
  display.showArt("+-+\n| |\n");
  EXPECT_EQ("+-+\n| |\n", out.str());
}

TEST(ConsoleDisplayTest, ClearWritesClearCode) {
  std::ostringstream out;
  ConsoleDisplay display(out);
  display.clear();
  EXPECT_EQ("\033[2J\033[H", out.str());
}

TEST(ConsoleInputTest, ReadsLines) {
  std::istringstream in("n\nquit\n");
  ConsoleInput input(in);
  EXPECT_EQ("n", input.readLine());
  EXPECT_EQ("quit", input.readLine());
}

TEST(ConsoleInputTest, EofReturnsQuit) {
  std::istringstream in("");
  ConsoleInput input(in);
  EXPECT_EQ("quit", input.readLine());
  EXPECT_EQ("quit", input.readLine());
}

TEST(ConsoleInputTest, StripsCarriageReturn) {
  std::istringstream in("west\r\n");
  ConsoleInput input(in);
  EXPECT_EQ("west", input.readLine());
}
