/**
 * @file Room.hpp
 * @brief A place in the world: a description, exits, items and a scene.
 */
#ifndef ROOM_HPP_INCLUDED
#define ROOM_HPP_INCLUDED

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "Item.hpp"
#include "SceneKind.hpp"

/**
 * @brief Abstract base for every location. Subclasses change what they show
 * and which entries they allow; the base supplies safe defaults.
 */
class Room {
 public:
  /**
   * @brief Creates a room.
   * @param id Stable room id, e.g. "saloon"; also the picture file name.
   * @param name Shown as the heading, e.g. "Saloon".
   * @param description Base description text.
   * @param kind Picture type for this room when lit.
   */
  Room(const std::string& id, const std::string& name,
       const std::string& description, SceneKind kind);

  /** @brief Virtual destructor. Owns its items. */
  virtual ~Room();

  /** @brief Stable room id. */
  const std::string& getId() const;

  /** @brief Heading shown on the screen. */
  const std::string& getName() const;

  /** @brief Picture type to show right now. */
  virtual SceneKind sceneKind() const;

  /** @brief Adds an exit. Names are stored normalized (lowercase, trimmed). */
  void setExit(const std::string& name, Room* to);

  /** @brief The room reached by an exit name, or nullptr. */
  Room* getExit(const std::string& name) const;

  /** @brief Exit names in the order they were added. */
  std::vector<std::string> exitNames() const;

  /** @brief Puts an item on the floor. Takes ownership. */
  void addItem(std::unique_ptr<Item> item);

  /** @brief Removes and returns the item matching the word, or nullptr. */
  std::unique_ptr<Item> removeItem(const std::string& word);

  /** @brief The item matching the word, or nullptr. Does not remove it. */
  const Item* findItem(const std::string& word) const;

  /** @brief True if items on the floor can be seen right now. */
  virtual bool itemsVisible() const;

  /** @brief Full text shown when the player looks around. */
  virtual std::string describe() const = 0;

  /**
   * @brief Whether the player may enter.
   * @param why Set to the reason when entry is refused.
   */
  virtual bool canEnter(std::string& why) const;

  /** @brief Message for a move in a direction with no usable exit. */
  virtual std::string noExitMessage(const std::string& exit) const;

 protected:
  /** @brief Base description text. */
  const std::string& description() const;

  /** @brief Description, items and exits, one per line. */
  std::string standardText() const;

  /** @brief "You see a tin cup." or "" when the floor is empty. */
  std::string itemsLine() const;

  /** @brief "Exits: north and out." */
  std::string exitsLine() const;

  /** @brief The picture type this room has when lit. */
  SceneKind baseSceneKind() const;

 private:
  std::string id_;
  std::string name_;
  std::string description_;
  SceneKind kind_;
  std::vector<std::pair<std::string, Room*>> exits_;
  std::vector<std::unique_ptr<Item>> items_;
};

#endif  // ROOM_HPP_INCLUDED
