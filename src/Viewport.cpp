#include "Viewport.hpp"

#include <algorithm>

namespace {

int clampedStart(int target, int viewSize, int mapSize) {
  const int maxStart = std::max(0, mapSize - viewSize);
  return std::min(std::max(target - viewSize / 2, 0), maxStart);
}

}  // namespace

Viewport::Viewport(int width, int height)
    : width_(width), height_(height), origin_(0, 0) {}

int Viewport::width() const { return width_; }

int Viewport::height() const { return height_; }

void Viewport::follow(const Point& target, int mapWidth, int mapHeight) {
  origin_ = Point(clampedStart(target.x, width_, mapWidth),
                  clampedStart(target.y, height_, mapHeight));
}

Point Viewport::origin() const { return origin_; }

Point Viewport::toScreen(const Point& world) const {
  return Point(world.x - origin_.x, world.y - origin_.y);
}
