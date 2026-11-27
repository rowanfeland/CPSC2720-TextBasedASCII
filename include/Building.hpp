/**
 * @file Building.hpp
 * @brief A building on an outdoor map: its footprint, door and sign.
 */
#ifndef BUILDING_HPP_INCLUDED
#define BUILDING_HPP_INCLUDED

#include <string>
#include "Geometry.hpp"

/**
 * @brief Where a building stands and where its door is.
 *
 * The footprint and door decide collision. The id is also the art asset key,
 * so replacing art never changes where the walls are.
 */
struct Building {
  std::string id;     ///< Stable asset ID, e.g. "general_store".
  std::string label;  ///< Sign text drawn on the placeholder box.
  Rect footprint;     ///< Cells the building blocks.
  Point door;         ///< Walkable door cell on the footprint's edge.

  /** @brief Creates a building. */
  Building(const std::string& buildingId, const std::string& signLabel,
           const Rect& area, const Point& doorCell)
      : id(buildingId), label(signLabel), footprint(area), door(doorCell) {}
};

#endif  // BUILDING_HPP_INCLUDED
