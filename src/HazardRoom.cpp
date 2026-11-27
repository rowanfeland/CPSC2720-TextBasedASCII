#include "HazardRoom.hpp"

#include <string>

#include "Room.hpp"
#include "SceneKind.hpp"

HazardRoom::HazardRoom(const std::string& id, const std::string& name,
                       const std::string& description, SceneKind kind,
                       const std::string& unease)
    : Room(id, name, description, kind), unease_(unease) {}

std::string HazardRoom::describe() const {
  return standardText() + "\n" + unease_;
}
