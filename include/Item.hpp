/**
 * @file Item.hpp
 * @brief An object the player can carry, examine, and leave behind.
 */
#ifndef ITEM_HPP_INCLUDED
#define ITEM_HPP_INCLUDED

#include <string>

/** @brief A named object with a description. */
class Item {
 public:
  /** @brief Creates an item. */
  Item(const std::string& name, const std::string& description);

  /** @brief Virtual destructor for subclasses. */
  virtual ~Item();

  /** @brief The item's name, e.g. "tin cup". */
  const std::string& getName() const;

  /** @brief The description shown on examine. */
  const std::string& getDescription() const;

  /** @brief Text shown when the player examines the item. */
  virtual std::string examine() const;

  /**
   * @brief True if the word refers to this item: the full name or its last
   * word (so "cup" matches "tin cup"). Case-insensitive.
   */
  bool matches(const std::string& word) const;

 private:
  std::string name_;
  std::string description_;
};

#endif  // ITEM_HPP_INCLUDED
