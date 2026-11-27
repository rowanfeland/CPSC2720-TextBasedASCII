#include "Camera.hpp"

#include <cmath>

#include "WorldMap.hpp"

namespace {

const double kTwoPi = 6.283185307179586;
const double kFieldOfView = 0.66;  // camera plane length; about 66 degrees
const double kRadius = 0.2;        // space kept clear around the player
const double kMinMove = 1e-6;      // smaller moves are rounding, not steps

/** True if a player centred at (x, y) has room to stand there. */
bool fits(const WorldMap& map, double x, double y) {
  return !map.isBlocked(x - kRadius, y - kRadius) &&
         !map.isBlocked(x + kRadius, y - kRadius) &&
         !map.isBlocked(x - kRadius, y + kRadius) &&
         !map.isBlocked(x + kRadius, y + kRadius);
}

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

bool Camera::step(const WorldMap& map, double distance) {
  const double startX = x_;
  const double startY = y_;
  const double nx = x_ + dirX() * distance;
  const double ny = y_ + dirY() * distance;
  if (fits(map, nx, y_)) {
    x_ = nx;
  }
  if (fits(map, x_, ny)) {
    y_ = ny;
  }
  return std::hypot(x_ - startX, y_ - startY) > kMinMove;
}
