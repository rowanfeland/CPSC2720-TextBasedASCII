#include "TerminalPainter.hpp"

#include <cstdio>
#include <string>

#include "FrameBuffer.hpp"
#include "Rgb.hpp"

namespace {

const char kHalfBlock[] = "\xE2\x96\x80";
const char kReset[] = "\033[0m";
const char kRamp[] = " .:-=+*#%@";
const int kRampTop = 9;

int cellRows(const FrameBuffer& f) { return (f.height() + 1) / 2; }

std::string moveTo(int row, int col) {
  char buf[32];
  std::snprintf(buf, sizeof(buf), "\033[%d;%dH", row, col);
  return buf;
}

std::string colourCode(int layer, const Rgb& c) {
  char buf[32];
  std::snprintf(buf, sizeof(buf), "\033[%d;2;%d;%d;%dm", layer, c.r, c.g, c.b);
  return buf;
}

bool cellDiffers(const FrameBuffer& a, const FrameBuffer& b, int x, int row) {
  return a.get(x, 2 * row) != b.get(x, 2 * row) ||
         a.get(x, 2 * row + 1) != b.get(x, 2 * row + 1);
}

/**
 * Appends cells [from, to) of one cell row. Colour codes are sent only when
 * they change, since the terminal keeps the last colour set.
 */
void appendCells(std::string& out, const FrameBuffer& f, int row, int from,
                 int to) {
  const int kForeground = 38;
  const int kBackground = 48;
  for (int x = from; x < to; ++x) {
    const Rgb top = f.get(x, 2 * row);
    const Rgb bottom = f.get(x, 2 * row + 1);
    if (x == from || top != f.get(x - 1, 2 * row)) {
      out += colourCode(kForeground, top);
    }
    if (x == from || bottom != f.get(x - 1, 2 * row + 1)) {
      out += colourCode(kBackground, bottom);
    }
    out += kHalfBlock;
  }
}

}  // namespace

std::string TerminalPainter::paintFull(const FrameBuffer& frame, int topRow) {
  std::string out;
  for (int row = 0; row < cellRows(frame); ++row) {
    out += moveTo(topRow + row, 1);
    appendCells(out, frame, row, 0, frame.width());
    out += kReset;
  }
  return out;
}

std::string TerminalPainter::paintDiff(const FrameBuffer& prev,
                                       const FrameBuffer& next, int topRow) {
  if (prev.width() != next.width() || prev.height() != next.height()) {
    return paintFull(next, topRow);
  }
  std::string out;
  for (int row = 0; row < cellRows(next); ++row) {
    int x = 0;
    while (x < next.width()) {
      if (!cellDiffers(prev, next, x, row)) {
        ++x;
        continue;
      }
      int end = x;
      while (end < next.width() && cellDiffers(prev, next, end, row)) {
        ++end;
      }
      out += moveTo(topRow + row, x + 1);
      appendCells(out, next, row, x, end);
      x = end;
    }
  }
  if (!out.empty()) {
    out += kReset;
  }
  return out;
}

std::string TerminalPainter::paintPlain(const FrameBuffer& frame) {
  std::string out;
  for (int row = 0; row < cellRows(frame); ++row) {
    for (int x = 0; x < frame.width(); ++x) {
      const Rgb a = frame.get(x, 2 * row);
      const Rgb b = frame.get(x, 2 * row + 1);
      const int lum = (a.r * 3 + a.g * 6 + a.b + b.r * 3 + b.g * 6 + b.b) / 20;
      out += kRamp[lum * kRampTop / 255];
    }
    out += '\n';
  }
  return out;
}
