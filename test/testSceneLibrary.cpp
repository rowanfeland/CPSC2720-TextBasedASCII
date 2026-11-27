#include <gtest/gtest.h>

#include "GreyImage.hpp"
#include "SceneKind.hpp"
#include "SceneLibrary.hpp"

namespace {

double rowAverage(const GreyImage& img, int y) {
  double sum = 0;
  for (int x = 0; x < img.width(); ++x) {
    sum += img.get(x, y);
  }
  return sum / img.width();
}

}  // namespace

TEST(SceneLibraryTest, PlaceholderSizes) {
  for (SceneKind kind : {SceneKind::Outdoor, SceneKind::Indoor, SceneKind::Dark}) {
    GreyImage img = makePlaceholder(kind);
    EXPECT_EQ(kSceneWidth, img.width());
    EXPECT_EQ(kSceneHeight, img.height());
  }
}

TEST(SceneLibraryTest, DarkIsDark) {
  GreyImage img = makePlaceholder(SceneKind::Dark);
  for (int y = 0; y < img.height(); ++y) {
    for (int x = 0; x < img.width(); ++x) {
      EXPECT_LE(img.get(x, y), 20);
    }
  }
}

TEST(SceneLibraryTest, OutdoorSkyBrighterThanGround) {
  GreyImage img = makePlaceholder(SceneKind::Outdoor);
  EXPECT_GT(rowAverage(img, 2), rowAverage(img, kSceneHeight - 2));
}

TEST(SceneLibraryTest, IndoorNotFlat) {
  GreyImage img = makePlaceholder(SceneKind::Indoor);
  EXPECT_NE(img.get(0, 0), img.get(kSceneWidth / 2, kSceneHeight / 2));
}

TEST(SceneLibraryTest, LoadsAndHalves) {
  SceneLibrary lib;
  ASSERT_TRUE(lib.loadFromDirectory("test/data/scenes"));
  EXPECT_TRUE(lib.has("big"));
  GreyImage img = lib.sceneFor("big", SceneKind::Outdoor);
  EXPECT_EQ(kSceneWidth, img.width());
  EXPECT_EQ(kSceneHeight, img.height());
  EXPECT_EQ(200, img.get(80, 20));
}

TEST(SceneLibraryTest, BadFileFallsBack) {
  SceneLibrary lib;
  ASSERT_TRUE(lib.loadFromDirectory("test/data/scenes"));
  EXPECT_FALSE(lib.has("broken"));
  GreyImage img = lib.sceneFor("broken", SceneKind::Dark);
  EXPECT_EQ(kSceneWidth, img.width());
  EXPECT_EQ(kSceneHeight, img.height());
}

TEST(SceneLibraryTest, MissingDirectory) {
  SceneLibrary lib;
  EXPECT_FALSE(lib.loadFromDirectory("test/data/nothing-here"));
}

TEST(SceneLibraryTest, UnknownKeyPlaceholder) {
  SceneLibrary lib;
  GreyImage img = lib.sceneFor("saloon", SceneKind::Indoor);
  EXPECT_EQ(kSceneWidth, img.width());
  EXPECT_EQ(kSceneHeight, img.height());
}
