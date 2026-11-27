#include "LoopRoom.hpp"

#include <string>

#include "Room.hpp"
#include "SceneKind.hpp"
#include "TextUtil.hpp"

LoopRoom::LoopRoom(const std::string& id, const std::string& name,
                   const std::string& description, SceneKind kind,
                   const std::string& onwardExit,
                   const std::string& onwardMessage)
    : Room(id, name, description, kind),
      onwardExit_(normalize(onwardExit)),
      onwardMessage_(onwardMessage) {}

std::string LoopRoom::noExitMessage(const std::string& exit) const {
  if (normalize(exit) == onwardExit_) {
    return onwardMessage_;
  }
  return Room::noExitMessage(exit);
}

std::string LoopRoom::describe() const { return standardText(); }
