/**
 * @file LoopRoom.hpp
 * @brief A road that leads back to where you started until the story allows.
 */
#ifndef LOOPROOM_HPP_INCLUDED
#define LOOPROOM_HPP_INCLUDED

#include <string>

#include "Room.hpp"
#include "SceneKind.hpp"

/** @brief One exit leads nowhere yet; it gives a fixed message instead. */
class LoopRoom : public Room {
 public:
  /**
   * @param onwardExit Exit name that is blocked for now, e.g. "east".
   * @param onwardMessage Message shown when that exit is used.
   * See Room for the other parameters.
   */
  LoopRoom(const std::string& id, const std::string& name,
           const std::string& description, SceneKind kind,
           const std::string& onwardExit, const std::string& onwardMessage);

  /** @brief The blocked exit returns its message; others use the default. */
  std::string noExitMessage(const std::string& exit) const override;

  /** @brief Standard text. */
  std::string describe() const override;

 private:
  std::string onwardExit_;
  std::string onwardMessage_;
};

#endif  // LOOPROOM_HPP_INCLUDED
