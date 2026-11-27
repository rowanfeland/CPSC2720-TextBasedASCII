/**
 * @file WorldMap.hpp
 * @brief The outdoor 3D world: tiles, buildings, doors and zones.
 */
#ifndef WORLDMAP_HPP_INCLUDED
#define WORLDMAP_HPP_INCLUDED

#include <map>
#include <string>
#include <vector>

#include "Geometry.hpp"
#include "Rgb.hpp"
#include "SkyModel.hpp"
#include "Sprite.hpp"

/** @brief What the ground looks like on a tile. */
enum class Ground { Sand, Street, Boardwalk, Grass, Salt };

/** @brief A position in tile units; (x + 0.5, y + 0.5) is a tile's centre. */
struct Point2 {
  double x;  ///< East-west position.
  double y;  ///< North-south position (grows southward).

  /** @brief Creates a point. */
  explicit Point2(double px = 0, double py = 0) : x(px), y(py) {}
};

/** @brief One building (or other enterable structure) from buildings.txt. */
struct BuildingDef {
  char letter;         ///< Map letter marking its footprint.
  std::string id;      ///< Stable id, e.g. "saloon".
  std::string label;   ///< Sign text.
  std::string style;   ///< Wall look: planks, adobe, barn, church, ...
  std::string roomId;  ///< Location opened by "enter".
  double height;       ///< Wall height in tiles.
  Rgb base;            ///< Main wall colour.
  Rgb trim;            ///< Frames, sign edges, roofline.
  int doorX;           ///< Tile just outside the door.
  int doorY;           ///< Tile just outside the door.
  char facing;         ///< Front wall faces 'N', 'S', 'E' or 'W'.
};

/** @brief A named area of the map that counts as one location. */
struct Zone {
  std::string roomId;       ///< Room the player is "in" here.
  Rect area;                ///< Tiles covered.
  std::string description;  ///< Shown by "look".
};

/** @brief A sprite placed in the world (headstone, cactus, barrel, ...). */
struct Prop {
  std::string sprite;  ///< Sprite name: data/world/sprites/<name>.txt.
  Point2 pos;          ///< Where it stands.
  bool blocking;       ///< Whether it stops the player.
};

/** @brief Blocking props stop the player within this distance. */
const double kPropRadius = 0.3;

/**
 * @brief The tile grid plus everything placed on it. Collision, zones and
 * doors come only from here, never from art.
 */
class WorldMap {
 public:
  /** @brief An empty 0 x 0 map. */
  WorldMap();

  /** @brief Columns of tiles. */
  int width() const;

  /** @brief Rows of tiles. */
  int height() const;

  /** @brief The map character at (x, y); '#' (rock) outside the map. */
  char tile(int x, int y) const;

  /** @brief Replaces the tile grid. Rows must all be the same length. */
  void setTiles(const std::vector<std::string>& rows);

  /** @brief Sets where the player starts. */
  void setStart(const Point2& p);

  /** @brief Where the player starts. */
  Point2 start() const;

  /** @brief True for rock, fences, buildings and anything off the map. */
  bool isBlocked(double x, double y) const;

  /** @brief Ground type under a position. */
  Ground groundAt(double x, double y) const;

  /** @brief The building occupying a tile, or nullptr. */
  const BuildingDef* buildingAt(int x, int y) const;

  /** @brief The first zone containing the position, or nullptr. */
  const Zone* zoneAt(double x, double y) const;

  /** @brief The closest building whose door is within range, or nullptr. */
  const BuildingDef* doorNear(double x, double y, double range) const;

  /** @brief Adds a building definition. */
  void addBuilding(const BuildingDef& b);

  /** @brief Adds a zone (earlier zones win where they overlap). */
  void addZone(const Zone& z);

  /** @brief All building definitions. */
  const std::vector<BuildingDef>& buildings() const;

  /** @brief All zones, in priority order. */
  const std::vector<Zone>& zones() const;

  /** @brief Places a prop. */
  void addProp(const Prop& p);

  /** @brief All placed props. */
  const std::vector<Prop>& props() const;

  /** @brief Adds or replaces a named sprite. */
  void addSprite(const std::string& name, const Sprite& s);

  /** @brief The sprite with this name, or nullptr. */
  const Sprite* sprite(const std::string& name) const;

  /** @brief Replaces the sky keyframes. */
  void setSky(const SkyModel& sky);

  /** @brief Sky colours and light through the day. */
  const SkyModel& sky() const;

 private:
  std::vector<std::string> rows_;
  int width_;
  int height_;
  Point2 start_;
  std::vector<BuildingDef> buildings_;
  std::vector<Zone> zones_;
  std::vector<Prop> props_;
  std::map<std::string, Sprite> sprites_;
  SkyModel sky_;
};

#endif  // WORLDMAP_HPP_INCLUDED
