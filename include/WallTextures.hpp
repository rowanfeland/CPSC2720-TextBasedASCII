/**
 * @file WallTextures.hpp
 * @brief Procedural wall looks for buildings, rock and fences.
 */
#ifndef WALLTEXTURES_HPP_INCLUDED
#define WALLTEXTURES_HPP_INCLUDED

#include "Rgb.hpp"
#include "WorldMap.hpp"

/**
 * @brief Colour of one point on a wall.
 * @param b The building (or rock/fence definition) that was hit.
 * @param u Position along the wall in tiles (world x for north/south walls,
 *          world y for east/west walls).
 * @param v Height above the ground in tiles (0 = ground).
 * @param facade True when this is the building's front wall (door, windows, sign).
 * @param night True after dark: some windows glow.
 * @param glows Set true when the texel gives off its own light (lit window),
 *              so the caller should not darken it.
 */
Rgb wallTexel(const BuildingDef& b, double u, double v, bool facade, bool night,
              bool& glows);

/** @brief Definition used for '#' rock tiles (the northern hills). */
const BuildingDef& rockWall();

/** @brief Definition used for 'F' fence tiles. */
const BuildingDef& fenceWall();

#endif  // WALLTEXTURES_HPP_INCLUDED
