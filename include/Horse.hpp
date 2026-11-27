/**
 * @file Horse.hpp
 * @brief The player's horse: where it is and whether you are riding it.
 */
#ifndef HORSE_HPP_INCLUDED
#define HORSE_HPP_INCLUDED

#include <string>

#include "WorldMap.hpp"

/** @brief Eye height when riding (tiles above the ground). */
const double kRidingEyeHeight = 0.72;
/** @brief Eye height on foot. */
const double kWalkingEyeHeight = 0.5;
/** @brief How close you must stand to mount. */
const double kMountRange = 1.5;

/** @brief Horse state. Riding doubles your step and raises your view. */
class Horse {
 public:
  /** @brief A horse at a position, ridden or not. */
  Horse(const Point2& position, bool mounted);

  /** @brief True while you are in the saddle. */
  bool isMounted() const;

  /** @brief Where the horse stands. */
  Point2 position() const;

  /** @brief Step multiplier: 2 riding, 1 on foot. */
  double speedFactor() const;

  /** @brief Eye height for the current state. */
  double eyeHeight() const;

  /** @brief Climbs into the saddle if the horse is within kMountRange. */
  std::string mount(const Point2& rider);

  /**
   * @brief Gets down. The horse stays just to your right, or under you if
   * that spot is blocked.
   * @param facing Your facing angle in radians.
   */
  std::string dismount(const WorldMap& map, const Point2& rider, double facing);

  /** @brief While ridden, the horse moves with you. */
  void follow(const Point2& rider);

 private:
  Point2 position_;
  bool mounted_;
};

#endif  // HORSE_HPP_INCLUDED
