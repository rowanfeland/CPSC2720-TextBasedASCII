/**
 * @file SceneRenderer.hpp
 * @brief Turns a greyscale image into terminal text, with survey digits and
 * erasure that depend on the day.
 */
#ifndef SCENERENDERER_HPP_INCLUDED
#define SCENERENDERER_HPP_INCLUDED

#include <string>

#include "GreyImage.hpp"

/** @brief How the picture is printed. */
enum class ColourMode {
  Grey,   ///< Each cell is a 256-colour grey background.
  Plain,  ///< Density characters only, for terminals without colour.
};

/** @brief Percent of cells showing survey digits on a given day. */
int digitPercent(int day);

/** @brief Percent of cells erased to flat white on a given day. */
int erasePercent(int day);

/**
 * @brief Renders the image one row per line.
 * Digits and erasure are chosen per cell from a fixed hash, so the same day
 * always looks the same.
 * @return Rows joined with newlines; Grey rows end with a colour reset.
 */
std::string renderScene(const GreyImage& img, int day, ColourMode mode);

#endif  // SCENERENDERER_HPP_INCLUDED
