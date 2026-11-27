#include <gtest/gtest.h>

#include <sstream>
#include <string>
#include <vector>

#include "AsciiArtLibrary.hpp"
#include "Renderer.hpp"
#include "SceneMap.hpp"
#include "Viewport.hpp"

namespace {

std::vector<std::string> lines(const std::string& frame) {
  std::vector<std::string> out;
  std::istringstream in(frame);
  std::string line;
  while (std::getline(in, line)) {
    out.push_back(line);
  }
  return out;
}

SceneMap shedMap() {
  SceneMap map("Test", 12, 6);
  map.addBuilding(Building("shed", "SHED", Rect(1, 0, 6, 3), Point(3, 2)));
  map.addRoad(4);
  return map;
}

}  // namespace

TEST(RendererTest, FrameSizeIsViewportPlusStatus) {
  AsciiArtLibrary art;
  Viewport vp(8, 4);
  Renderer r(art, vp);
  std::vector<std::string> out =
      lines(r.renderFrame(shedMap(), std::vector<Entity>(), Point(0, 0), "ok"));
  ASSERT_EQ(5u, out.size());
  for (size_t i = 0; i < 4; ++i) {
    EXPECT_EQ(8u, out[i].size());
  }
}

TEST(RendererTest, FallbackBox) {
  AsciiArtLibrary art;
  Viewport vp(12, 6);
  Renderer r(art, vp);
  std::vector<std::string> out =
      lines(r.renderFrame(shedMap(), std::vector<Entity>(), Point(0, 0), ""));
  EXPECT_EQ(" +----+     ", out[0]);
  EXPECT_EQ(" |SHED|     ", out[1]);
  EXPECT_EQ(" +-D--+     ", out[2]);
  EXPECT_EQ("============", out[4]);
}

TEST(RendererTest, DrawsPlayerAtFocus) {
  AsciiArtLibrary art;
  Viewport vp(4, 2);
  Renderer r(art, vp);
  std::vector<Entity> entities;
  entities.push_back(Entity("player", Point(10, 5), '@'));
  std::vector<std::string> out =
      lines(r.renderFrame(shedMap(), entities, Point(10, 5), ""));
  // Viewport clamps to origin (8, 4), so the player is at screen (2, 1).
  EXPECT_EQ('@', out[1][2]);
  EXPECT_EQ("====", out[0]);
}

TEST(RendererTest, ArtClippedToFootprint) {
  AsciiArtLibrary art;
  art.addArt("shed", "XXXXXXXXXX\nXXXXXXXXXX\nXXXXXXXXXX\nXXXXXXXXXX");
  Viewport vp(12, 6);
  Renderer r(art, vp);
  std::vector<std::string> out =
      lines(r.renderFrame(shedMap(), std::vector<Entity>(), Point(0, 0), ""));
  EXPECT_EQ(" XXXXXX     ", out[0]);
  EXPECT_EQ(" XXXXXX     ", out[1]);
  EXPECT_EQ(" XXDXXX     ", out[2]);
  EXPECT_EQ("            ", out[3]);
}

TEST(RendererTest, EmptyArtFallsBackToBox) {
  AsciiArtLibrary art;
  art.addArt("shed", "");
  Viewport vp(12, 6);
  Renderer r(art, vp);
  std::vector<std::string> out =
      lines(r.renderFrame(shedMap(), std::vector<Entity>(), Point(0, 0), ""));
  EXPECT_EQ(" +----+     ", out[0]);
  EXPECT_EQ(" |SHED|     ", out[1]);
}

TEST(RendererTest, StatusLineLastAndTruncated) {
  AsciiArtLibrary art;
  Viewport vp(6, 2);
  Renderer r(art, vp);
  std::vector<std::string> out = lines(
      r.renderFrame(shedMap(), std::vector<Entity>(), Point(0, 0),
                    "Main Street"));
  ASSERT_EQ(3u, out.size());
  EXPECT_EQ("Main S", out[2]);
}
