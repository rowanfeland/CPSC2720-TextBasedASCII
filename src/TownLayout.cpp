#include "TownLayout.hpp"

#include <string>

namespace {

const int kMapWidth = 90;
const int kMapHeight = 13;
const int kRoadRow = 6;
const int kNorthRowTop = 1;
const int kSouthRowTop = 9;
const int kBuildingHeight = 3;

/** North-side buildings face the road with a door on their bottom edge. */
Building northBuilding(const std::string& id, const std::string& label, int x,
                       int width) {
  return Building(id, label, Rect(x, kNorthRowTop, width, kBuildingHeight),
                  Point(x + width / 2, kNorthRowTop + kBuildingHeight - 1));
}

/** South-side buildings face the road with a door on their top edge. */
Building southBuilding(const std::string& id, const std::string& label, int x,
                       int width) {
  return Building(id, label, Rect(x, kSouthRowTop, width, kBuildingHeight),
                  Point(x + width / 2, kSouthRowTop));
}

}  // namespace

SceneMap TownLayout::buildMainStreet() {
  SceneMap map("Main Street", kMapWidth, kMapHeight);
  map.addRoad(kRoadRow);

  map.addBuilding(northBuilding("hotel", "HOTEL", 2, 9));
  map.addBuilding(northBuilding("saloon", "SALOON", 13, 10));
  map.addBuilding(northBuilding("gunsmith", "GUNSMITH", 25, 10));
  map.addBuilding(northBuilding("sheriff", "SHERIFF", 37, 10));
  map.addBuilding(northBuilding("stable", "STABLE", 49, 8));

  map.addBuilding(southBuilding("church", "CHURCH", 2, 9));
  map.addBuilding(southBuilding("general_store", "GENERAL STORE", 13, 15));
  map.addBuilding(southBuilding("telegraph", "TELEGRAPH", 30, 11));
  map.addBuilding(southBuilding("graves", "GRAVES", 43, 9));
  return map;
}

Point TownLayout::startPosition() { return Point(kMapWidth - 2, kRoadRow); }
