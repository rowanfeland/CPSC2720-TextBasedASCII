/**
 * @file World.hpp
 * @brief Owns every room and knows where the player starts.
 */
#ifndef WORLD_HPP_INCLUDED
#define WORLD_HPP_INCLUDED

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "Room.hpp"

/** @brief The collection of rooms. Owns them; rooms refer to each other by pointer. */
class World {
 public:
  /** @brief Creates an empty world with no start room. */
  World();

  /** @brief Destroys every room. */
  ~World();

  /** @brief Takes ownership of a room and returns it. */
  Room* addRoom(std::unique_ptr<Room> room);

  /** @brief The room with this id, or nullptr. */
  Room* getRoom(const std::string& id) const;

  /**
   * @brief Links two rooms in both directions: aExit leads from a to b,
   * bExit leads from b to a.
   */
  static void connect(Room* a, const std::string& aExit, Room* b,
                      const std::string& bExit);

  /** @brief Sets where the player begins. */
  void setStartRoom(Room* room);

  /** @brief Where the player begins, or nullptr if not set. */
  Room* getStartRoom() const;

  /** @brief Number of rooms. */
  int roomCount() const;

  /** @brief Every room, in the order added. */
  std::vector<Room*> rooms() const;

 private:
  std::vector<std::unique_ptr<Room>> rooms_;
  std::map<std::string, Room*> byId_;
  Room* start_;
};

#endif  // WORLD_HPP_INCLUDED
