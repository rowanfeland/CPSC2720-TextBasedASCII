#include "Item.hpp"

#include <string>

#include "TextUtil.hpp"

Item::Item(const std::string& name, const std::string& description)
    : name_(name), description_(description) {}

Item::~Item() {}

const std::string& Item::getName() const { return name_; }

const std::string& Item::getDescription() const { return description_; }

std::string Item::examine() const { return description_; }

bool Item::matches(const std::string& word) const {
  const std::string w = normalize(word);
  if (w.empty()) {
    return false;
  }
  const std::string name = normalize(name_);
  if (w == name) {
    return true;
  }
  const std::string::size_type space = name.rfind(' ');
  const std::string last =
      space == std::string::npos ? name : name.substr(space + 1);
  return w == last;
}
