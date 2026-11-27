#include "FrameBuffer.hpp"

#include <algorithm>
#include <vector>

#include "Rgb.hpp"

FrameBuffer::FrameBuffer(int width, int height)
    : width_(std::max(0, width)),
      height_(std::max(0, height)),
      pixels_(static_cast<std::vector<Rgb>::size_type>(width_) * height_) {}

int FrameBuffer::width() const { return width_; }

int FrameBuffer::height() const { return height_; }

Rgb FrameBuffer::get(int x, int y) const {
  if (x < 0 || x >= width_ || y < 0 || y >= height_) {
    return Rgb();
  }
  return pixels_[static_cast<std::vector<Rgb>::size_type>(y) * width_ + x];
}

void FrameBuffer::set(int x, int y, const Rgb& c) {
  if (x < 0 || x >= width_ || y < 0 || y >= height_) {
    return;
  }
  pixels_[static_cast<std::vector<Rgb>::size_type>(y) * width_ + x] = c;
}

void FrameBuffer::fill(const Rgb& c) {
  std::fill(pixels_.begin(), pixels_.end(), c);
}
