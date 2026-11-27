/**
 * @file Sprite.hpp
 * @brief Pixel-art pictures for things standing in the world (horse, props).
 */
#ifndef SPRITE_HPP_INCLUDED
#define SPRITE_HPP_INCLUDED

#include <map>
#include <string>
#include <vector>

#include "Rgb.hpp"

/**
 * @brief A small picture with a size in world tiles. Each row is a string of
 * palette letters; '.' and any letter missing from the palette are see-through.
 */
struct Sprite {
  double width;                ///< Width in tiles.
  double height;               ///< Height in tiles.
  std::vector<std::string> rows;   ///< Pixel rows, all the same length.
  std::map<char, Rgb> palette;     ///< Letter to colour.

  /** @brief An empty 0 x 0 sprite. */
  Sprite();

  /** @brief Pixel columns. */
  int columns() const;

  /** @brief Pixel rows. */
  int rowCount() const;

  /** @brief Colour at (col, row); false where see-through or outside. */
  bool pixel(int col, int row, Rgb& out) const;
};

/**
 * @brief Reads a sprite file: "size W H", then "X = r g b" palette lines,
 * then pixel rows. Lines starting with '#' are comments.
 * @param why Set to the reason when the text is rejected.
 */
bool parseSprite(const std::string& text, Sprite& out, std::string& why);

#endif  // SPRITE_HPP_INCLUDED
