#include "StandardRoom.hpp"

#include <string>

#include "Room.hpp"
#include "SceneKind.hpp"

StandardRoom::StandardRoom(const std::string& id, const std::string& name,
                           const std::string& description, SceneKind kind)
    : Room(id, name, description, kind) {}

std::string StandardRoom::describe() const { return standardText(); }
