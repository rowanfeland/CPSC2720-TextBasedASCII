#include "SceneRenderer.hpp"

#include <algorithm>
#include <cstdio>
#include <string>

namespace {

const char kRamp[] = " .:-=+*#%@";
const int kRampLevels = 9;
const int kBayer[4][4] = {{0, 8, 2, 10}, {12, 4, 14, 6}, {3, 11, 1, 9}, {15, 7, 13, 5}};

/** Stable pseudo-random value for a cell. */
unsigned cellHash(int x, int y) {
  return (static_cast<unsigned>(x) * 73856093u) ^ (static_cast<unsigned>(y) * 19349663u);
}

int greyIndex(int v) { return 232 + (v * 23 + 127) / 255; }

std::string plainChar(int v, int x, int y) {
  const int scaled = v * kRampLevels;
  const int base = scaled / 255;
  const int rem = scaled % 255;
  const int threshold = (kBayer[y % 4][x % 4] * 255 + 127) / 16;
  const int level = std::min(kRampLevels, base + (rem > threshold ? 1 : 0));
  return std::string(1, kRamp[level]);
}

}  // namespace

int digitPercent(int day) {
  if (day <= 1) {
    return 0;
  }
  return std::min(60, (day - 1) * 8);
}

int erasePercent(int day) {
  if (day < 5) {
    return 0;
  }
  if (day == 5) {
    return 10;
  }
  if (day == 6) {
    return 25;
  }
  if (day == 7) {
    return 45;
  }
  return 70;
}

std::string renderScene(const GreyImage& img, int day, ColourMode mode) {
  std::string out;
  for (int y = 0; y < img.height(); ++y) {
    for (int x = 0; x < img.width(); ++x) {
      const unsigned h = cellHash(x, y);
      int v = img.get(x, y);
      const bool erased = static_cast<int>(h % 100) < erasePercent(day);
      if (erased) {
        v = 255;
      }
      const bool digit = !erased && static_cast<int>((h / 100) % 100) < digitPercent(day);
      const char digitChar = static_cast<char>('0' + (h / 10000) % 10);

      if (mode == ColourMode::Plain) {
        if (erased) {
          out += '@';
        } else if (digit) {
          out += digitChar;
        } else {
          out += plainChar(v, x, y);
        }
      } else {
        const int bg = greyIndex(v);
        const int fgShift = v < 128 ? 7 : -7;
        const int fg = std::min(255, std::max(232, bg + fgShift));
        char glyph = ' ';
        if (digit) {
          glyph = digitChar;
        }
        char code[64];
        std::snprintf(code, sizeof(code), "\033[48;5;%dm\033[38;5;%dm", bg, fg);
        out += code;
        out += glyph;
      }
    }
    if (mode == ColourMode::Grey) {
      out += "\033[0m";
    }
    out += '\n';
  }
  return out;
}
