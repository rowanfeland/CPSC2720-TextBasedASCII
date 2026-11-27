/**
 * @file Raycaster.hpp
 * @brief Renders the first-person view of the outdoor world into a FrameBuffer.
 */
#ifndef RAYCASTER_HPP_INCLUDED
#define RAYCASTER_HPP_INCLUDED

#include <vector>

#include "Camera.hpp"
#include "FrameBuffer.hpp"
#include "Rgb.hpp"
#include "WorldMap.hpp"

/** @brief Horizon row as a fraction of the frame height. */
const double kHorizonFraction = 0.43;
/** @brief Colour of the sun disc. */
const Rgb kSunColour(255, 236, 186);
/** @brief Colour of the moon disc. */
const Rgb kMoonColour(230, 230, 214);

/** @brief Everything about the sky and light for one moment of the day. */
struct SkyState {
  Rgb top;              ///< Sky colour at the top of the view.
  Rgb horizon;          ///< Sky (and fog) colour at the horizon.
  double light;         ///< 1 = full daylight, near 0 = deep night.
  double sunAzimuth;    ///< Direction of the sun or moon (radians, 0 = east).
  double sunElevation;  ///< 0 = on the horizon, 1 = high overhead.
  bool sunVisible;      ///< Whether to draw a disc at all.
  bool moon;            ///< Draw the moon instead of the sun.
  double stars;         ///< 0 = no stars, 1 = full night sky.

  /** @brief Default: sunset, sun low in the west. */
  SkyState();
};

/** @brief What to draw: the world, where the eye is, and the sky. */
struct RenderInput {
  const WorldMap* map;  ///< The world (not owned).
  Camera cam;           ///< Eye position and facing.
  double eyeHeight;     ///< 0.5 on foot, higher when riding.
  SkyState sky;         ///< Time-of-day look.
  bool riding;          ///< Draw the horse's ears, mane and reins at the bottom.
  bool showHorse;       ///< Draw the horse standing in the world.
  Point2 horse;         ///< Where the horse stands (when shown).

  /** @brief On-foot view of a map from a camera, at sunset. */
  RenderInput(const WorldMap& world, const Camera& camera);
};

/**
 * @brief Draws sky, sun or moon, stars, distant mountains, the ground, all
 * walls, every prop sprite, the horse and (when riding) the riding overlay.
 * Pure: the same input always gives the same pixels.
 * @param depth Resized to width x height; holds wall distance per pixel
 *              (very large where no wall was drawn), for sprites.
 */
void renderView(const RenderInput& in, FrameBuffer& frame,
                std::vector<double>& depth);

#endif  // RAYCASTER_HPP_INCLUDED
