/**
 * @file Pgm.hpp
 * @brief Reads PGM (portable graymap) images, the format used for scene art.
 */
#ifndef PGM_HPP_INCLUDED
#define PGM_HPP_INCLUDED

#include <string>

#include "GreyImage.hpp"

/**
 * @brief Parses P2 (text) or P5 (binary) PGM data. Comments are allowed.
 * Values are scaled to 0..255 using the file's maximum value.
 * @return false if the header is wrong, the size is not positive, or data is short.
 */
bool parsePgm(const std::string& data, GreyImage& out);

/** @brief Reads a PGM file from disk. Returns false if it can't be read or parsed. */
bool loadPgm(const std::string& path, GreyImage& out);

#endif  // PGM_HPP_INCLUDED
