#include "World.hpp"

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "Room.hpp"

World::World() : start_(nullptr) {}

World::~World() {}

Room* World::addRoom(std::unique_ptr<Room> room) {
  rooms_.push_back(std::move(room));
  Room* stored = rooms_.back().get();
  byId_[stored->getId()] = stored;
  return stored;
}

Room* World::getRoom(const std::string& id) const {
  std::map<std::string, Room*>::const_iterator it = byId_.find(id);
  return it == byId_.end() ? nullptr : it->second;
}

void World::connect(Room* a, const std::string& aExit, Room* b,
                    const std::string& bExit) {
  a->setExit(aExit, b);
  b->setExit(bExit, a);
}

void World::setStartRoom(Room* room) { start_ = room; }

Room* World::getStartRoom() const { return start_; }

int World::roomCount() const { return static_cast<int>(rooms_.size()); }

std::vector<Room*> World::rooms() const {
  std::vector<Room*> out;
  for (std::vector<std::unique_ptr<Room>>::const_iterator it = rooms_.begin();
       it != rooms_.end(); ++it) {
    out.push_back(it->get());
  }
  return out;
}
