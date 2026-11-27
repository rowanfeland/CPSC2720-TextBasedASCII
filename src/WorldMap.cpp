#include "WorldMap.hpp"

#include <cmath>
#include <map>
#include <string>
#include <vector>

#include "Sprite.hpp"

namespace {

int cellOf(double v) { return static_cast<int>(std::floor(v)); }

}  // namespace

WorldMap::WorldMap() : width_(0), height_(0) {}

int WorldMap::width() const { return width_; }

int WorldMap::height() const { return height_; }

char WorldMap::tile(int x, int y) const {
  if (x < 0 || y < 0 || x >= width_ || y >= height_) {
    return '#';
  }
  return rows_[y][x];
}

void WorldMap::setTiles(const std::vector<std::string>& rows) {
  rows_ = rows;
  height_ = static_cast<int>(rows.size());
  width_ = rows.empty() ? 0 : static_cast<int>(rows[0].size());
}

void WorldMap::setStart(const Point2& p) { start_ = p; }

Point2 WorldMap::start() const { return start_; }

bool WorldMap::isBlocked(double x, double y) const {
  const int cx = cellOf(x);
  const int cy = cellOf(y);
  const char c = tile(cx, cy);
  if (c == '#' || c == 'F' || buildingAt(cx, cy) != nullptr) {
    return true;
  }
  for (std::vector<Prop>::size_type i = 0; i < props_.size(); ++i) {
    if (props_[i].blocking &&
        std::hypot(props_[i].pos.x - x, props_[i].pos.y - y) < kPropRadius) {
      return true;
    }
  }
  return false;
}

Ground WorldMap::groundAt(double x, double y) const {
  switch (tile(cellOf(x), cellOf(y))) {
    case '=':
      return Ground::Street;
    case '_':
      return Ground::Boardwalk;
    case ',':
      return Ground::Grass;
    case '~':
      return Ground::Salt;
    default:
      return Ground::Sand;
  }
}

const BuildingDef* WorldMap::buildingAt(int x, int y) const {
  const char c = tile(x, y);
  if (c < 'a' || c > 'z') {
    return nullptr;
  }
  for (std::vector<BuildingDef>::size_type i = 0; i < buildings_.size(); ++i) {
    if (buildings_[i].letter == c) {
      return &buildings_[i];
    }
  }
  return nullptr;
}

const Zone* WorldMap::zoneAt(double x, double y) const {
  const Point p(cellOf(x), cellOf(y));
  for (std::vector<Zone>::size_type i = 0; i < zones_.size(); ++i) {
    if (zones_[i].area.contains(p)) {
      return &zones_[i];
    }
  }
  return nullptr;
}

const BuildingDef* WorldMap::doorNear(double x, double y, double range) const {
  const BuildingDef* best = nullptr;
  double bestDist = range;
  for (std::vector<BuildingDef>::size_type i = 0; i < buildings_.size(); ++i) {
    const double d = std::hypot(buildings_[i].doorX + 0.5 - x,
                                buildings_[i].doorY + 0.5 - y);
    if (d <= bestDist) {
      bestDist = d;
      best = &buildings_[i];
    }
  }
  return best;
}

void WorldMap::addBuilding(const BuildingDef& b) { buildings_.push_back(b); }

void WorldMap::addZone(const Zone& z) { zones_.push_back(z); }

const std::vector<BuildingDef>& WorldMap::buildings() const {
  return buildings_;
}

const std::vector<Zone>& WorldMap::zones() const { return zones_; }

void WorldMap::addProp(const Prop& p) { props_.push_back(p); }

const std::vector<Prop>& WorldMap::props() const { return props_; }

void WorldMap::addSprite(const std::string& name, const Sprite& s) {
  sprites_[name] = s;
}

void WorldMap::setSky(const SkyModel& sky) { sky_ = sky; }

const SkyModel& WorldMap::sky() const { return sky_; }

const Sprite* WorldMap::sprite(const std::string& name) const {
  const std::map<std::string, Sprite>::const_iterator it = sprites_.find(name);
  return it == sprites_.end() ? nullptr : &it->second;
}
