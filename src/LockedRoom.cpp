#include "LockedRoom.hpp"

#include <string>

#include "Room.hpp"
#include "SceneKind.hpp"

LockedRoom::LockedRoom(const std::string& id, const std::string& name,
                       const std::string& description, SceneKind kind,
                       const std::string& keyId)
    : Room(id, name, description, kind), keyId_(keyId), locked_(true) {}

bool LockedRoom::unlock(const std::string& keyId) {
  if (keyId == keyId_) {
    locked_ = false;
  }
  return !locked_;
}

bool LockedRoom::isLocked() const { return locked_; }

bool LockedRoom::canEnter(std::string& why) const {
  if (locked_) {
    why = "The door is locked. You need a key.";
    return false;
  }
  return true;
}

std::string LockedRoom::describe() const { return standardText(); }
