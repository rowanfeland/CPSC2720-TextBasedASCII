/**
 * @file FrameBuffer.hpp
 * @brief A grid of colour pixels that one view frame is drawn into.
 */
#ifndef FRAMEBUFFER_HPP_INCLUDED
#define FRAMEBUFFER_HPP_INCLUDED

#include <vector>

#include "Rgb.hpp"

/**
 * @brief Width-by-height colour pixels. Off-image reads return black and
 * off-image writes are ignored.
 */
class FrameBuffer {
 public:
  /** @brief Creates a black frame (negative sizes become 0). */
  FrameBuffer(int width, int height);

  /** @brief Number of pixel columns. */
  int width() const;

  /** @brief Number of pixel rows. */
  int height() const;

  /** @brief Pixel at (x, y), or black when outside. */
  Rgb get(int x, int y) const;

  /** @brief Sets one pixel; ignored when outside. */
  void set(int x, int y, const Rgb& c);

  /** @brief Sets every pixel to one colour. */
  void fill(const Rgb& c);

 private:
  int width_;
  int height_;
  std::vector<Rgb> pixels_;
};

#endif  // FRAMEBUFFER_HPP_INCLUDED
