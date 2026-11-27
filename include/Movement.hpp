/**
 * @file Movement.hpp
 * @brief Compass directions and one-cell movement on a SceneMap.
 */
#ifndef MOVEMENT_HPP_INCLUDED
#define MOVEMENT_HPP_INCLUDED

#include <string>

#include "Geometry.hpp"
#include "SceneMap.hpp"

/** @brief A compass direction, or None for input that is not one. */
enum class Direction { North, South, East, West, None };

/**
 * @brief Reads a direction word. Ignores case and surrounding whitespace.
 * Accepts n/north, s/south, e/east, w/west.
 * @return The direction, or Direction::None if the word is not one.
 */
Direction parseDirection(const std::string& word);

/** @brief The neighbouring cell in a direction (None returns the same cell). */
Point step(const Point& from, Direction direction);

/**
 * @brief Moves one cell if the destination is walkable.
 * @param map Map to check against.
 * @param position Updated only when the move succeeds.
 * @param direction Where to go.
 * @return true if the position changed.
 */
bool tryMove(const SceneMap& map, Point& position, Direction direction);

#endif  // MOVEMENT_HPP_INCLUDED
