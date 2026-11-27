/**
 * @file Canvas.hpp
 * @brief A fixed-size grid of characters that frames are drawn into.
 */
#ifndef CANVAS_HPP_INCLUDED
#define CANVAS_HPP_INCLUDED

#include <string>
#include <vector>
#include "Geometry.hpp"

/**
 * @brief A width-by-height grid of characters.
 *
 * Every drawing call clips silently: anything outside the grid is ignored,
 * so callers never need to bounds-check.
 */
class Canvas {
 public:
  /**
   * @brief Creates a canvas filled with one character.
   * @param width Columns (negative is treated as 0).
   * @param height Rows (negative is treated as 0).
   * @param fill Starting character for every cell.
   */
  Canvas(int width, int height, char fill = ' ');

  /** @brief Number of columns. */
  int width() const;

  /** @brief Number of rows. */
  int height() const;

  /** @brief Sets one cell. Ignored when (x, y) is outside the canvas. */
  void put(int x, int y, char c);

  /** @brief Returns one cell, or a space when (x, y) is outside the canvas. */
  char get(int x, int y) const;

  /** @brief Writes text left to right starting at (x, y), clipped. */
  void drawText(int x, int y, const std::string& text);

  /**
   * @brief Draws a box outline: '+' corners, '-' top and bottom, '|' sides.
   * The interior is left untouched.
   */
  void drawBox(const Rect& r);

  /**
   * @brief Copies a multi-line block with its top-left at (x, y).
   * Spaces in the block are transparent and leave the canvas as it was.
   */
  void blit(int x, int y, const std::string& block);

  /** @brief All rows joined, each ending with a newline. */
  std::string toString() const;

 private:
  bool inBounds(int x, int y) const;

  int width_;
  int height_;
  std::vector<std::string> rows_;
};

#endif  // CANVAS_HPP_INCLUDED
