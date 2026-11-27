/**
 * @file GreyImage.hpp
 * @brief A greyscale picture: a grid of 0 (black) to 255 (white) values.
 */
#ifndef GREYIMAGE_HPP_INCLUDED
#define GREYIMAGE_HPP_INCLUDED

#include <vector>

#include "Geometry.hpp"

/**
 * @brief Width-by-height grid of greyscale values. Off-image reads return 0
 * and off-image writes are ignored.
 */
class GreyImage {
 public:
  /**
   * @brief Creates an image filled with one value.
   * @param width Columns (negative is treated as 0).
   * @param height Rows (negative is treated as 0).
   * @param fill Starting value, clamped to 0..255.
   */
  explicit GreyImage(int width, int height, int fill = 0);

  /** @brief Number of columns. */
  int width() const;

  /** @brief Number of rows. */
  int height() const;

  /** @brief Value at (x, y), or 0 when outside the image. */
  int get(int x, int y) const;

  /** @brief Sets a value, clamped to 0..255. Ignored outside the image. */
  void set(int x, int y, int value);

  /** @brief Fills a rectangle (clipped to the image) with one value. */
  void fillRect(const Rect& r, int value);

  /** @brief Half-size copy; each output pixel is the average of a 2x2 block. */
  GreyImage halved() const;

 private:
  int width_;
  int height_;
  std::vector<int> pixels_;
};

#endif  // GREYIMAGE_HPP_INCLUDED
