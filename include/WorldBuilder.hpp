/**
 * @file WorldBuilder.hpp
 * @brief Builds the full 18-location town from the design table.
 */
#ifndef WORLDBUILDER_HPP_INCLUDED
#define WORLDBUILDER_HPP_INCLUDED

#include <memory>

#include "World.hpp"

/** @brief Creates the whole world: rooms, types, exits, items and start room. */
class WorldBuilder {
 public:
  /**
   * @brief Builds the world. The caller owns the result.
   * Start room: desert_road. Evidence room starts unlocked for now.
   */
  static std::unique_ptr<World> build();
};

#endif  // WORLDBUILDER_HPP_INCLUDED
