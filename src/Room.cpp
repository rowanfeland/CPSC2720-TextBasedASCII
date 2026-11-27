#include "Room.hpp"

#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "Item.hpp"
#include "SceneKind.hpp"
#include "TextUtil.hpp"

Room::Room(const std::string& id, const std::string& name,
           const std::string& description, SceneKind kind)
    : id_(id), name_(name), description_(description), kind_(kind) {}

Room::~Room() {}

const std::string& Room::getId() const { return id_; }

const std::string& Room::getName() const { return name_; }

SceneKind Room::sceneKind() const { return kind_; }

void Room::setExit(const std::string& name, Room* to) {
  exits_.push_back(std::make_pair(normalize(name), to));
}

Room* Room::getExit(const std::string& name) const {
  const std::string key = normalize(name);
  std::vector<std::pair<std::string, Room*>>::const_iterator it = std::find_if(
      exits_.begin(), exits_.end(),
      [&key](const std::pair<std::string, Room*>& exit) { return exit.first == key; });
  return it == exits_.end() ? nullptr : it->second;
}

std::vector<std::string> Room::exitNames() const {
  std::vector<std::string> out;
  for (std::vector<std::pair<std::string, Room*>>::const_iterator it = exits_.begin();
       it != exits_.end(); ++it) {
    out.push_back(it->first);
  }
  return out;
}

void Room::addItem(std::unique_ptr<Item> item) {
  items_.push_back(std::move(item));
}

std::unique_ptr<Item> Room::removeItem(const std::string& word) {
  std::vector<std::unique_ptr<Item>>::iterator it = std::find_if(
      items_.begin(), items_.end(),
      [&word](const std::unique_ptr<Item>& item) { return item->matches(word); });
  if (it == items_.end()) {
    return nullptr;
  }
  std::unique_ptr<Item> out = std::move(*it);
  items_.erase(it);
  return out;
}

const Item* Room::findItem(const std::string& word) const {
  std::vector<std::unique_ptr<Item>>::const_iterator it = std::find_if(
      items_.begin(), items_.end(),
      [&word](const std::unique_ptr<Item>& item) { return item->matches(word); });
  return it == items_.end() ? nullptr : it->get();
}

bool Room::itemsVisible() const { return true; }

bool Room::canEnter(std::string& why) const {
  (void)why;
  return true;
}

std::string Room::noExitMessage(const std::string& exit) const {
  (void)exit;
  return "You can't go that way.";
}

const std::string& Room::description() const { return description_; }

std::string Room::standardText() const {
  std::string out = description_;
  const std::string items = itemsLine();
  if (!items.empty()) {
    out += "\n" + items;
  }
  out += "\n" + exitsLine();
  return out;
}

std::string Room::itemsLine() const {
  if (items_.empty() || !itemsVisible()) {
    return "";
  }
  std::vector<std::string> names;
  for (std::vector<std::unique_ptr<Item>>::const_iterator it = items_.begin();
       it != items_.end(); ++it) {
    const std::string name = (*it)->getName();
    const bool vowel = !name.empty() && std::string("aeiou").find(name[0]) != std::string::npos;
    names.push_back((vowel ? "an " : "a ") + name);
  }
  return "You see " + joinList(names) + ".";
}

std::string Room::exitsLine() const {
  if (exits_.empty()) {
    return "There are no obvious exits.";
  }
  return "Exits: " + joinList(exitNames()) + ".";
}

SceneKind Room::baseSceneKind() const { return kind_; }
