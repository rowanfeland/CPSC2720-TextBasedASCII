#include "Player.hpp"

#include "Inventory.hpp"
#include "Room.hpp"

Player::Player(Room* start, int capacity) : room_(start), inventory_(capacity) {}

Room* Player::getCurrentRoom() const { return room_; }

void Player::moveTo(Room* room) { room_ = room; }

Inventory& Player::getInventory() { return inventory_; }
