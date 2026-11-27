/**
 * @file Player.hpp
 * @brief The character the player controls: where they are and what they carry.
 */
#ifndef PLAYER_HPP_INCLUDED
#define PLAYER_HPP_INCLUDED

#include "Inventory.hpp"
#include "Room.hpp"

/** @brief The player's position and inventory. */
class Player {
 public:
  /**
   * @param start Room the player begins in.
   * @param capacity How many items the player can carry.
   */
  Player(Room* start, int capacity);

  /** @brief The room the player is in. */
  Room* getCurrentRoom() const;

  /** @brief Moves the player to another room. */
  void moveTo(Room* room);

  /** @brief The player's inventory. */
  Inventory& getInventory();

 private:
  Room* room_;
  Inventory inventory_;
};

#endif  // PLAYER_HPP_INCLUDED
