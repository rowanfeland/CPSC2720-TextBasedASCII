/**
 * @file LockedRoom.hpp
 * @brief A room whose door needs a key before anyone can enter.
 */
#ifndef LOCKEDROOM_HPP_INCLUDED
#define LOCKEDROOM_HPP_INCLUDED

#include <string>

#include "Room.hpp"
#include "SceneKind.hpp"

/** @brief Blocks entry until the matching key unlocks it. */
class LockedRoom : public Room {
 public:
  /**
   * @param keyId Item id of the key that opens this door, e.g. "iron_key".
   * See Room for the other parameters.
   */
  LockedRoom(const std::string& id, const std::string& name,
             const std::string& description, SceneKind kind,
             const std::string& keyId);

  /** @brief Unlocks if the key matches. Returns true if it is now unlocked. */
  bool unlock(const std::string& keyId);

  /** @brief True while the door is still locked. */
  bool isLocked() const;

  /** @brief Refuses entry while locked, with the reason. */
  bool canEnter(std::string& why) const override;

  /** @brief Standard text. */
  std::string describe() const override;

 private:
  std::string keyId_;
  bool locked_;
};

#endif  // LOCKEDROOM_HPP_INCLUDED
