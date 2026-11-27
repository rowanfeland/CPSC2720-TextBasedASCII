#include "Horse.hpp"

#include <cmath>
#include <string>

#include "WorldMap.hpp"

namespace {

const double kSideOffset = 0.9;  // how far to your side the horse stands
const double kHalfPi = 1.5707963267948966;

}  // namespace

Horse::Horse(const Point2& position, bool mounted)
    : position_(position), mounted_(mounted) {}

bool Horse::isMounted() const { return mounted_; }

Point2 Horse::position() const { return position_; }

double Horse::speedFactor() const { return mounted_ ? 2.0 : 1.0; }

double Horse::eyeHeight() const {
  return mounted_ ? kRidingEyeHeight : kWalkingEyeHeight;
}

std::string Horse::mount(const Point2& rider) {
  if (mounted_) {
    return "You're already riding.";
  }
  if (std::hypot(rider.x - position_.x, rider.y - position_.y) > kMountRange) {
    return "Your horse is too far away.";
  }
  mounted_ = true;
  position_ = rider;
  return "You swing up into the saddle.";
}

std::string Horse::dismount(const WorldMap& map, const Point2& rider,
                            double facing) {
  if (!mounted_) {
    return "You aren't riding.";
  }
  mounted_ = false;
  const Point2 beside(rider.x + std::cos(facing + kHalfPi) * kSideOffset,
                      rider.y + std::sin(facing + kHalfPi) * kSideOffset);
  position_ = map.isBlocked(beside.x, beside.y) ? rider : beside;
  return "You swing down from the saddle.";
}

void Horse::follow(const Point2& rider) {
  if (mounted_) {
    position_ = rider;
  }
}
