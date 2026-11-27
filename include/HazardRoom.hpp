/**
 * @file HazardRoom.hpp
 * @brief A place near the line, where something is quietly wrong.
 */
#ifndef HAZARDROOM_HPP_INCLUDED
#define HAZARDROOM_HPP_INCLUDED

#include <string>

#include "Room.hpp"
#include "SceneKind.hpp"

/** @brief Room with an extra unsettling line added to its description. */
class HazardRoom : public Room {
 public:
  /**
   * @param unease Extra line shown after the description.
   * See Room for the other parameters.
   */
  HazardRoom(const std::string& id, const std::string& name,
             const std::string& description, SceneKind kind,
             const std::string& unease);

  /** @brief Standard text plus the unease line. */
  std::string describe() const override;

 private:
  std::string unease_;
};

#endif  // HAZARDROOM_HPP_INCLUDED
