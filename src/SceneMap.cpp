#include "SceneMap.hpp"

#include <string>
#include <vector>

SceneMap::SceneMap(const std::string& name, int width, int height, char ground)
    : name_(name), width_(width), height_(height), ground_(ground) {}

const std::string& SceneMap::name() const { return name_; }

int SceneMap::width() const { return width_; }

int SceneMap::height() const { return height_; }

char SceneMap::ground() const { return ground_; }

void SceneMap::addBuilding(const Building& building) {
  buildings_.push_back(building);
}

const std::vector<Building>& SceneMap::buildings() const { return buildings_; }

void SceneMap::addRoad(int y) { roads_.insert(y); }

bool SceneMap::isRoad(int y) const { return roads_.count(y) > 0; }

bool SceneMap::isInside(const Point& p) const {
  return p.x >= 0 && p.x < width_ && p.y >= 0 && p.y < height_;
}

bool SceneMap::isWalkable(const Point& p) const {
  if (!isInside(p)) {
    return false;
  }
  for (std::vector<Building>::size_type i = 0; i < buildings_.size(); ++i) {
    const Building& b = buildings_[i];
    if (b.footprint.contains(p) && b.door != p) {
      return false;
    }
  }
  return true;
}

const Building* SceneMap::buildingAtDoor(const Point& p) const {
  for (std::vector<Building>::size_type i = 0; i < buildings_.size(); ++i) {
    if (buildings_[i].door == p) {
      return &buildings_[i];
    }
  }
  return nullptr;
}
