#include "Inventory.hpp"

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#include "TextUtil.hpp"

Inventory::Inventory(int capacity) : capacity_(capacity < 0 ? 0 : capacity) {}

std::unique_ptr<Item> Inventory::add(std::unique_ptr<Item> item) {
  if (isFull()) {
    return item;
  }
  items_.push_back(std::move(item));
  return nullptr;
}

std::unique_ptr<Item> Inventory::remove(const std::string& word) {
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

const Item* Inventory::find(const std::string& word) const {
  std::vector<std::unique_ptr<Item>>::const_iterator it = std::find_if(
      items_.begin(), items_.end(),
      [&word](const std::unique_ptr<Item>& item) { return item->matches(word); });
  return it == items_.end() ? nullptr : it->get();
}

bool Inventory::isFull() const {
  return static_cast<int>(items_.size()) >= capacity_;
}

int Inventory::size() const { return static_cast<int>(items_.size()); }

std::vector<std::string> Inventory::names() const {
  std::vector<std::string> out;
  for (std::vector<std::unique_ptr<Item>>::const_iterator it = items_.begin();
       it != items_.end(); ++it) {
    out.push_back((*it)->getName());
  }
  return out;
}

std::string Inventory::list() const {
  if (items_.empty()) {
    return "You are carrying nothing.";
  }
  return "You are carrying " + joinList(names()) + ".";
}
