#include "Camera.hpp"

#include <cmath>

#include "WorldMap.hpp"

namespace {

const double kTwoPi = 6.283185307179586;
const double kFieldOfView = 0.66;  // camera plane length; about 66 degrees
const double kRadius = 0.2;        // space kept clear around the player

}  // namespace

Camera::Camera(double x, double y, double angle) : x_(x), y_(y), angle_(0) {
  setAngle(angle);
}

double Camera::x() const { return x_; }

double Camera::y() const { return y_; }

double Camera::angle() const { return angle_; }

double Camera::dirX() const { return std::cos(angle_); }

double Camera::dirY() const { return std::sin(angle_); }

double Camera::planeX() const { return -dirY() * kFieldOfView; }

double Camera::planeY() const { return dirX() * kFieldOfView; }

void Camera::turn(double radians) { setAngle(angle_ + radians); }

void Camera::setAngle(double radians) {
  angle_ = std::fmod(radians, kTwoPi);
  if (angle_ < 0) {
    angle_ += kTwoPi;
  }
}

void Camera::setPosition(double x, double y) {
  x_ = x;
  y_ = y;
}

bool Camera::fits(const WorldMap& map, double x, double y) const {
  return !map.isBlocked(x - kRadius, y - kRadius) &&
         !map.isBlocked(x + kRadius, y - kRadius) &&
         !map.isBlocked(x - kRadius, y + kRadius) &&
         !map.isBlocked(x + kRadius, y + kRadius);
}

bool Camera::step(const WorldMap& map, double distance) {
  const double nx = x_ + dirX() * distance;
  const double ny = y_ + dirY() * distance;
  bool moved = false;
  if (fits(map, nx, y_)) {
    moved = moved || nx != x_;
    x_ = nx;
  }
  if (fits(map, x_, ny)) {
    moved = moved || ny != y_;
    y_ = ny;
  }
  return moved;
}
