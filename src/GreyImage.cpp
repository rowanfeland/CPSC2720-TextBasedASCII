#include "GreyImage.hpp"

#include <algorithm>
#include <vector>

namespace {

int clampGrey(int v) { return std::min(255, std::max(0, v)); }

}  // namespace

GreyImage::GreyImage(int width, int height, int fill)
    : width_(std::max(0, width)),
      height_(std::max(0, height)),
      pixels_(static_cast<std::vector<int>::size_type>(width_) * height_,
              clampGrey(fill)) {}

GreyImage::GreyImage() : GreyImage(0, 0) {}

int GreyImage::width() const { return width_; }

int GreyImage::height() const { return height_; }

int GreyImage::get(int x, int y) const {
  if (x < 0 || x >= width_ || y < 0 || y >= height_) {
    return 0;
  }
  return pixels_[static_cast<std::vector<int>::size_type>(y) * width_ + x];
}

void GreyImage::set(int x, int y, int value) {
  if (x < 0 || x >= width_ || y < 0 || y >= height_) {
    return;
  }
  pixels_[static_cast<std::vector<int>::size_type>(y) * width_ + x] =
      clampGrey(value);
}

void GreyImage::fillRect(const Rect& r, int value) {
  for (int y = r.y; y < r.y + r.height; ++y) {
    for (int x = r.x; x < r.x + r.width; ++x) {
      set(x, y, value);
    }
  }
}

GreyImage GreyImage::halved() const {
  GreyImage out(width_ / 2, height_ / 2);
  for (int y = 0; y < out.height(); ++y) {
    for (int x = 0; x < out.width(); ++x) {
      const int sum = get(2 * x, 2 * y) + get(2 * x + 1, 2 * y) +
                      get(2 * x, 2 * y + 1) + get(2 * x + 1, 2 * y + 1);
      out.set(x, y, sum / 4);
    }
  }
  return out;
}
