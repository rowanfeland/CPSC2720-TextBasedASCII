#include "DarkRoom.hpp"

#include <string>

#include "Room.hpp"
#include "SceneKind.hpp"

DarkRoom::DarkRoom(const std::string& id, const std::string& name,
                   const std::string& description, SceneKind kind)
    : Room(id, name, description, kind), lit_(false) {}

void DarkRoom::illuminate() { lit_ = true; }

bool DarkRoom::isLit() const { return lit_; }

SceneKind DarkRoom::sceneKind() const {
  return lit_ ? baseSceneKind() : SceneKind::Dark;
}

bool DarkRoom::itemsVisible() const { return lit_; }

std::string DarkRoom::describe() const {
  if (lit_) {
    return standardText();
  }
  return "It is too dark to see.\n" + exitsLine();
}
