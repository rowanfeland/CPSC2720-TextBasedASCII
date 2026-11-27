/**
 * @file Camera.hpp
 * @brief The player's eye in the 3D view: position, facing and movement.
 */
#ifndef CAMERA_HPP_INCLUDED
#define CAMERA_HPP_INCLUDED

#include "WorldMap.hpp"

/** @brief How far one arrow press turns, in radians (15 degrees). */
const double kTurnStep = 0.2617993877991494;
/** @brief How far one arrow press walks, in tiles. */
const double kWalkStep = 0.5;

/**
 * @brief Position and facing on the map. Angle 0 faces east; angles grow
 * clockwise because map y grows southward. Movement keeps a small radius
 * clear of walls and slides along them instead of stopping dead.
 */
class Camera {
 public:
  /** @brief Creates a camera at (x, y) facing angle (radians). */
  Camera(double x, double y, double angle);

  /** @brief East-west position. */
  double x() const;

  /** @brief North-south position. */
  double y() const;

  /** @brief Facing, in radians, 0 to 2 pi. */
  double angle() const;

  /** @brief Facing direction, x part. */
  double dirX() const;

  /** @brief Facing direction, y part. */
  double dirY() const;

  /** @brief Camera plane (field of view), x part. */
  double planeX() const;

  /** @brief Camera plane (field of view), y part. */
  double planeY() const;

  /** @brief Turns by radians (positive turns right). */
  void turn(double radians);

  /** @brief Sets the facing directly. */
  void setAngle(double radians);

  /** @brief Moves directly, without collision. */
  void setPosition(double x, double y);

  /**
   * @brief Walks forward (positive) or back (negative), sliding along walls.
   * @return true if the position changed.
   */
  bool step(const WorldMap& map, double distance);

 private:
  bool fits(const WorldMap& map, double x, double y) const;

  double x_;
  double y_;
  double angle_;
};

#endif  // CAMERA_HPP_INCLUDED
