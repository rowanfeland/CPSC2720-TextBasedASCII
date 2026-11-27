/**
 * @file StandardRoom.hpp
 * @brief An ordinary, lit room with a description, items and exits.
 */
#ifndef STANDARDROOM_HPP_INCLUDED
#define STANDARDROOM_HPP_INCLUDED

#include <string>

#include "Room.hpp"
#include "SceneKind.hpp"

/** @brief A plain room that shows its description, items and exits. */
class StandardRoom : public Room {
 public:
  /** @brief Creates a standard room. See Room for the parameters. */
  StandardRoom(const std::string& id, const std::string& name,
               const std::string& description, SceneKind kind);

  /** @brief Description, items and exits. */
  std::string describe() const override;
};

#endif  // STANDARDROOM_HPP_INCLUDED
