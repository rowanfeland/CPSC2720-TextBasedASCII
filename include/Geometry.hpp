/**
 * @file Geometry.hpp
 * @brief Small value types for terminal cell coordinates.
 */
#ifndef GEOMETRY_HPP_INCLUDED
#define GEOMETRY_HPP_INCLUDED

/**
 * @brief A cell position. x grows to the right (east), y grows down (south).
 */
struct Point {
  int x;  ///< Column.
  int y;  ///< Row.

  /** @brief Creates a point at (x, y). */
  explicit Point(int px = 0, int py = 0) : x(px), y(py) {}

  /** @brief True when both coordinates match. */
  bool operator==(const Point& other) const {
    return x == other.x && y == other.y;
  }

  /** @brief True when either coordinate differs. */
  bool operator!=(const Point& other) const { return !(*this == other); }
};

/**
 * @brief An axis-aligned rectangle of cells, from (x, y) spanning width by height.
 */
struct Rect {
  int x;       ///< Left column.
  int y;       ///< Top row.
  int width;   ///< Number of columns.
  int height;  ///< Number of rows.

  /** @brief Creates a rectangle. */
  explicit Rect(int rx = 0, int ry = 0, int w = 0, int h = 0)
      : x(rx), y(ry), width(w), height(h) {}

  /** @brief True when the cell lies inside the rectangle. */
  bool contains(const Point& p) const {
    return p.x >= x && p.x < x + width && p.y >= y && p.y < y + height;
  }

  /** @brief True when the two rectangles share at least one cell. */
  bool intersects(const Rect& o) const {
    return x < o.x + o.width && o.x < x + width &&
           y < o.y + o.height && o.y < y + height;
  }
};

#endif  // GEOMETRY_HPP_INCLUDED
