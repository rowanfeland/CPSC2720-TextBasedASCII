/**
 * @file Renderer.hpp
 * @brief Composes a map, its buildings and entities into one screen frame.
 */
#ifndef RENDERER_HPP_INCLUDED
#define RENDERER_HPP_INCLUDED

#include <string>
#include <vector>

#include "AsciiArtLibrary.hpp"
#include "Canvas.hpp"
#include "Entity.hpp"
#include "SceneMap.hpp"
#include "Viewport.hpp"

/**
 * @brief Draws frames in layers: ground and roads, then buildings, then
 * entities, then the status line.
 *
 * A building uses art from the library when its id has some (clipped to the
 * footprint); otherwise it is drawn as a plain box with its label.
 */
class Renderer {
 public:
  /**
   * @brief Creates a renderer.
   * @param art Art lookup; must outlive the renderer.
   * @param viewport View window; must outlive the renderer.
   */
  Renderer(const AsciiArtLibrary& art, Viewport& viewport);

  /**
   * @brief Builds one frame.
   * @param map Map to draw.
   * @param entities Sprites drawn on top, in order (later ones win).
   * @param focus Map cell the viewport centres on.
   * @param status Text for the bottom line, cut to the viewport width.
   * @return Viewport rows plus a status row, each ending in a newline.
   */
  std::string renderFrame(const SceneMap& map,
                          const std::vector<Entity>& entities,
                          const Point& focus, const std::string& status);

 private:
  void drawBuilding(Canvas& world, const Building& building) const;

  const AsciiArtLibrary& art_;
  Viewport& viewport_;
};

#endif  // RENDERER_HPP_INCLUDED
