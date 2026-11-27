/**
 * @file SkyState.hpp
 * @brief The look of the sky and the light at one moment of the day.
 */
#ifndef SKYSTATE_HPP_INCLUDED
#define SKYSTATE_HPP_INCLUDED

#include "Rgb.hpp"

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

#endif  // SKYSTATE_HPP_INCLUDED
