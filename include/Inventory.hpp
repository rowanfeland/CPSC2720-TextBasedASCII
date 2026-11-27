/**
 * @file Inventory.hpp
 * @brief The items the player is carrying, with a capacity limit.
 */
#ifndef INVENTORY_HPP_INCLUDED
#define INVENTORY_HPP_INCLUDED

#include <memory>
#include <string>
#include <vector>

#include "Item.hpp"

/** @brief Holds items up to a fixed capacity. Owns the items it stores. */
class Inventory {
 public:
  /** @brief Creates an empty inventory that holds at most capacity items. */
  explicit Inventory(int capacity);

  /**
   * @brief Stores an item.
   * @return nullptr if stored; otherwise the item, handed back because the
   * inventory is full.
   */
  std::unique_ptr<Item> add(std::unique_ptr<Item> item);

  /** @brief Removes and returns the item matching the word, or nullptr. */
  std::unique_ptr<Item> remove(const std::string& word);

  /** @brief The item matching the word, or nullptr. Does not remove it. */
  const Item* find(const std::string& word) const;

  /** @brief True when no more items fit. */
  bool isFull() const;

  /** @brief Number of items carried. */
  int size() const;

  /** @brief Names of the carried items, in the order picked up. */
  std::vector<std::string> names() const;

  /** @brief A sentence describing what is carried. */
  std::string list() const;

 private:
  int capacity_;
  std::vector<std::unique_ptr<Item>> items_;
};

#endif  // INVENTORY_HPP_INCLUDED
