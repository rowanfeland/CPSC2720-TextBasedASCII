/**
 * @file Entity.hpp
 * @brief Something drawn on top of the map: the player, an NPC, the horse.
 */
#ifndef ENTITY_HPP_INCLUDED
#define ENTITY_HPP_INCLUDED

#include <string>
#include "Geometry.hpp"

/**
 * @brief A one-cell placeholder sprite. The glyph is only for drawing;
 * the id stays the same when real art replaces the glyph.
 */
struct Entity {
  std::string id;  ///< Stable ID, e.g. "player" or "sheriff".
  Point pos;       ///< Map cell the entity stands on.
  char glyph;      ///< Placeholder character.

  /** @brief Creates an entity. */
  Entity(const std::string& entityId, const Point& position, char symbol)
      : id(entityId), pos(position), glyph(symbol) {}
};

#endif  // ENTITY_HPP_INCLUDED
