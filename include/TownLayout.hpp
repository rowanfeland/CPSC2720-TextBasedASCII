/**
 * @file TownLayout.hpp
 * @brief Builds the outdoor town maps.
 */
#ifndef TOWNLAYOUT_HPP_INCLUDED
#define TOWNLAYOUT_HPP_INCLUDED

#include "Geometry.hpp"
#include "SceneMap.hpp"

/**
 * @brief Creates the Main Street map: five buildings north of the road,
 * four south of it, and the road running east out into the desert.
 */
class TownLayout {
 public:
  /** @brief The Main Street map with all nine building blockouts. */
  static SceneMap buildMainStreet();

  /** @brief Where the player starts: on the road at the east edge. */
  static Point startPosition();
};

#endif  // TOWNLAYOUT_HPP_INCLUDED
