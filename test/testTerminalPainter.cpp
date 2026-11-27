#include <gtest/gtest.h>

#include <string>

#include "FrameBuffer.hpp"
#include "Rgb.hpp"
#include "TerminalPainter.hpp"

namespace {

const std::string kHalfBlock = "\xE2\x96\x80";

int count(const std::string& s, const std::string& what) {
  int n = 0;
  for (std::string::size_type at = s.find(what); at != std::string::npos;
       at = s.find(what, at + what.size())) {
    ++n;
  }
  return n;
}

}  // namespace

TEST(TerminalPainterTest, FullUsesHalfBlocks) {
  FrameBuffer fb(2, 2);
  fb.set(0, 0, Rgb(255, 0, 0));
  fb.set(0, 1, Rgb(0, 0, 255));
  const std::string out = TerminalPainter::paintFull(fb, 1);
  EXPECT_EQ(2, count(out, kHalfBlock));
  EXPECT_NE(std::string::npos, out.find("\033[38;2;255;0;0m"));
  EXPECT_NE(std::string::npos, out.find("\033[48;2;0;0;255m"));
  EXPECT_EQ(0u, out.find("\033[1;1H"));
}

TEST(TerminalPainterTest, DiffEmptyWhenSame) {
  FrameBuffer a(4, 4);
  FrameBuffer b(4, 4);
  EXPECT_EQ("", TerminalPainter::paintDiff(a, b, 1));
}

TEST(TerminalPainterTest, DiffOnlyChanged) {
  FrameBuffer a(4, 4);
  FrameBuffer b(4, 4);
  b.set(2, 3, Rgb(9, 9, 9));  // bottom half of cell row 1, column 2
  const std::string out = TerminalPainter::paintDiff(a, b, 1);
  EXPECT_EQ(1, count(out, kHalfBlock));
  EXPECT_EQ(1, count(out, "H"));
  EXPECT_NE(std::string::npos, out.find("\033[2;3H"));
}

TEST(TerminalPainterTest, DiffSizeChangeRepaintsAll) {
  FrameBuffer a(2, 2);
  FrameBuffer b(3, 2);
  EXPECT_EQ(TerminalPainter::paintFull(b, 1), TerminalPainter::paintDiff(a, b, 1));
}

TEST(TerminalPainterTest, PlainNoEscapes) {
  FrameBuffer fb(3, 4);
  fb.fill(Rgb(255, 255, 255));
  fb.set(0, 0, Rgb(0, 0, 0));
  fb.set(0, 1, Rgb(0, 0, 0));
  const std::string out = TerminalPainter::paintPlain(fb);
  EXPECT_EQ(std::string::npos, out.find('\033'));
  EXPECT_EQ(" @@\n@@@\n", out);
}

TEST(TerminalPainterTest, OddHeightPads) {
  FrameBuffer fb(1, 3);
  const std::string out = TerminalPainter::paintFull(fb, 1);
  EXPECT_EQ(2, count(out, kHalfBlock));
}
