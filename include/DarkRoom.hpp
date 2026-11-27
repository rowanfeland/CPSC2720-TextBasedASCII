/**
 * @file DarkRoom.hpp
 * @brief A room that is too dark to see until lit.
 */
#ifndef DARKROOM_HPP_INCLUDED
#define DARKROOM_HPP_INCLUDED

#include <string>

#include "Room.hpp"
#include "SceneKind.hpp"

/** @brief Unlit rooms hide their items and show a near-black picture. */
class DarkRoom : public Room {
 public:
  /** @brief Creates an unlit room. See Room for the parameters. */
  DarkRoom(const std::string& id, const std::string& name,
           const std::string& description, SceneKind kind);

  /** @brief Lights the room (called when a lantern is used). */
  void illuminate();

  /** @brief True once the room has been lit. */
  bool isLit() const;

  /** @brief Near-black picture until lit; the normal picture after. */
  SceneKind sceneKind() const override;

  /** @brief Items are hidden until lit. */
  bool itemsVisible() const override;

  /** @brief "It is too dark to see." when unlit; the normal text when lit. */
  std::string describe() const override;

 private:
  bool lit_;
};

#endif  // DARKROOM_HPP_INCLUDED
