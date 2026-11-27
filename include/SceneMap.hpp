/**
 * @file SceneMap.hpp
 * @brief Layout and collision for one map: size, roads and buildings.
 */
#ifndef SCENEMAP_HPP_INCLUDED
#define SCENEMAP_HPP_INCLUDED

#include <set>
#include <string>
#include <vector>

#include "Building.hpp"
#include "Geometry.hpp"

/**
 * @brief The walkable layout of one scene. Holds no art: the Renderer
 * decides how it looks, this class decides where you can go.
 */
class SceneMap {
 public:
  /**
   * @brief Creates an empty map.
   * @param name Shown in the status line.
   * @param width Columns.
   * @param height Rows.
   * @param ground Character drawn on open ground.
   */
  SceneMap(const std::string& name, int width, int height, char ground = ' ');

  /** @brief Map name. */
  const std::string& name() const;

  /** @brief Columns. */
  int width() const;

  /** @brief Rows. */
  int height() const;

  /** @brief Character drawn on open ground. */
  char ground() const;

  /** @brief Adds a building; its footprint blocks all but its door. */
  void addBuilding(const Building& building);

  /** @brief Every building, in the order added. */
  const std::vector<Building>& buildings() const;

  /** @brief Marks a whole row as road (drawn as '='; still walkable). */
  void addRoad(int y);

  /** @brief True if the row is road. */
  bool isRoad(int y) const;

  /** @brief True if the cell lies on the map. */
  bool isInside(const Point& p) const;

  /** @brief True if the cell is on the map and not blocked by a building. */
  bool isWalkable(const Point& p) const;

  /** @brief The building whose door is this cell, or nullptr. */
  const Building* buildingAtDoor(const Point& p) const;

 private:
  std::string name_;
  int width_;
  int height_;
  char ground_;
  std::vector<Building> buildings_;
  std::set<int> roads_;
};

#endif  // SCENEMAP_HPP_INCLUDED
