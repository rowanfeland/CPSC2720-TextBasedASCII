/**
 * @file WorldFiles.hpp
 * @brief Reads the editable world files in data/world/ into a WorldMap.
 *
 * Every parser takes the file's text, so it can be tested without files.
 * Bad lines are skipped with a warning; the game always starts.
 */
#ifndef WORLDFILES_HPP_INCLUDED
#define WORLDFILES_HPP_INCLUDED

#include <ostream>
#include <string>

#include "WorldMap.hpp"

/**
 * @brief Reads town.map: one character per tile. Short rows are padded with
 * sand; '@' marks the start (otherwise the first open tile is used).
 * @return false if there are no rows.
 */
bool parseTownMap(const std::string& text, WorldMap& map);

/**
 * @brief Reads buildings.txt and adds each good line.
 * @return Number of buildings added. Bad lines are reported on warn.
 */
int parseBuildings(const std::string& text, WorldMap& map, std::ostream& warn);

/**
 * @brief Reads zones.txt and adds each good line.
 * @return Number of zones added. Bad lines are reported on warn.
 */
int parseZones(const std::string& text, WorldMap& map, std::ostream& warn);

/**
 * @brief Reads props.txt ("sprite x y yes|no" per line) and adds each prop.
 * @return Number of props added. Bad lines are reported on warn.
 */
int parseProps(const std::string& text, WorldMap& map, std::ostream& warn);

/**
 * @brief Checks a loaded world: doors on blocked tiles are turned off
 * (with a warning naming the building).
 */
void validateWorld(WorldMap& map, std::ostream& warn);

/**
 * @brief Loads town.map, buildings.txt, zones.txt, props.txt and every .txt
 * file in the sprites folder.
 * Falls back to defaultWorld() (with a warning naming the folder) if
 * town.map is missing or empty.
 */
WorldMap loadWorld(const std::string& dir, std::ostream& warn);

/** @brief A small built-in street used when the world files are missing. */
WorldMap defaultWorld();

#endif  // WORLDFILES_HPP_INCLUDED
