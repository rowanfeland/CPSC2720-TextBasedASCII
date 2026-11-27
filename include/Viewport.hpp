/**
 * @file Viewport.hpp
 * @brief The window of the map that fits on screen.
 */
#ifndef VIEWPORT_HPP_INCLUDED
#define VIEWPORT_HPP_INCLUDED

#include "Geometry.hpp"

/**
 * @brief A width-by-height window onto a larger map that follows a target.
 */
class Viewport {
 public:
  /** @brief Creates a viewport of the given size in cells, at the map origin. */
  Viewport(int width, int height);

  /** @brief Columns shown. */
  int width() const;

  /** @brief Rows shown. */
  int height() const;

  /**
   * @brief Centres on the target, clamped so the view never leaves the map.
   * If the map is smaller than the view, the origin stays at (0, 0).
   */
  void follow(const Point& target, int mapWidth, int mapHeight);

  /** @brief Map cell shown in the top-left corner of the screen. */
  Point origin() const;

  /** @brief Converts a map cell to its screen cell. */
  Point toScreen(const Point& world) const;

 private:
  int width_;
  int height_;
  Point origin_;
};

#endif  // VIEWPORT_HPP_INCLUDED
