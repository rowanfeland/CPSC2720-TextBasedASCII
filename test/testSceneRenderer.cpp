#include <gtest/gtest.h>

#include <string>

#include "GreyImage.hpp"
#include "SceneRenderer.hpp"

namespace {

int countChar(const std::string& s, char c) {
  int n = 0;
  for (std::string::size_type i = 0; i < s.size(); ++i) {
    if (s[i] == c) {
      ++n;
    }
  }
  return n;
}

GreyImage solid(int w, int h, int v) {
  GreyImage img(w, h);
  img.fillRect(Rect(0, 0, w, h), v);
  return img;
}

}  // namespace

TEST(SceneRendererTest, DigitAndEraseTables) {
  EXPECT_EQ(0, digitPercent(1));
  EXPECT_EQ(8, digitPercent(2));
  EXPECT_EQ(32, digitPercent(5));
  EXPECT_EQ(48, digitPercent(7));
  EXPECT_EQ(60, digitPercent(9));
  EXPECT_EQ(0, erasePercent(4));
  EXPECT_EQ(10, erasePercent(5));
  EXPECT_EQ(25, erasePercent(6));
  EXPECT_EQ(45, erasePercent(7));
  EXPECT_EQ(70, erasePercent(9));
}

TEST(SceneRendererTest, PlainKnownRamp) {
  EXPECT_EQ(" \n", renderScene(solid(1, 1, 0), 1, ColourMode::Plain));
  EXPECT_EQ("@\n", renderScene(solid(1, 1, 255), 1, ColourMode::Plain));
}

TEST(SceneRendererTest, PlainHasNoEscapes) {
  std::string out = renderScene(solid(80, 20, 128), 7, ColourMode::Plain);
  EXPECT_EQ(std::string::npos, out.find('\033'));
}

TEST(SceneRendererTest, GreyCodes) {
  std::string black = renderScene(solid(1, 1, 0), 1, ColourMode::Grey);
  std::string white = renderScene(solid(1, 1, 255), 1, ColourMode::Grey);
  EXPECT_NE(std::string::npos, black.find("48;5;232m"));
  EXPECT_NE(std::string::npos, white.find("48;5;255m"));
  EXPECT_EQ("\033[0m\n", white.substr(white.size() - 5));
}

TEST(SceneRendererTest, RowCount) {
  std::string out = renderScene(solid(10, 7, 100), 1, ColourMode::Plain);
  EXPECT_EQ(7, countChar(out, '\n'));
}

TEST(SceneRendererTest, CleanOnDayOne) {
  std::string out = renderScene(solid(80, 20, 128), 1, ColourMode::Plain);
  for (std::string::size_type i = 0; i < out.size(); ++i) {
    EXPECT_TRUE(out[i] < '0' || out[i] > '9');
  }
}

TEST(SceneRendererTest, DigitsCreepIn) {
  std::string out = renderScene(solid(80, 20, 128), 7, ColourMode::Plain);
  // Digits only land on cells that are not erased (about 55% on day 7),
  // at 48% of those: roughly a quarter of the whole picture.
  int cells = 80 * 20;
  int digits = 0;
  for (char c = '0'; c <= '9'; ++c) {
    digits += countChar(out, c);
  }
  EXPECT_GT(digits, cells * 20 / 100);
  EXPECT_LT(digits, cells * 35 / 100);
}

TEST(SceneRendererTest, ErasureOnDayFive) {
  EXPECT_EQ(0, countChar(renderScene(solid(80, 20, 0), 4, ColourMode::Plain), '@'));
  EXPECT_GT(countChar(renderScene(solid(80, 20, 0), 5, ColourMode::Plain), '@'), 0);
}

TEST(SceneRendererTest, Deterministic) {
  GreyImage img = solid(40, 10, 90);
  EXPECT_EQ(renderScene(img, 6, ColourMode::Grey),
            renderScene(img, 6, ColourMode::Grey));
}
