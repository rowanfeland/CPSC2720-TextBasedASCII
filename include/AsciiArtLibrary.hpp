/**
 * @file AsciiArtLibrary.hpp
 * @brief Loads ASCII art blocks from text files and returns them by key.
 */
#ifndef ASCIIARTLIBRARY_HPP_INCLUDED
#define ASCIIARTLIBRARY_HPP_INCLUDED

#include <map>
#include <string>

/**
 * @brief Stores ASCII art keyed by asset ID (the file name without ".txt").
 *
 * Art is only ever looked at for drawing. Collision and doors live in
 * SceneMap, so swapping art never changes how the game plays.
 */
class AsciiArtLibrary {
 public:
  /**
   * @brief Loads every .txt file in a folder, keyed by file name.
   * Carriage returns are stripped so Windows line endings draw correctly.
   * @param path Folder to read.
   * @return false if the folder cannot be opened.
   */
  bool loadFromDirectory(const std::string& path);

  /** @brief Adds or replaces one art block. */
  void addArt(const std::string& key, const std::string& art);

  /** @brief Returns the art for a key, or an empty string if there is none. */
  std::string getArt(const std::string& key) const;

  /** @brief True if art exists for the key. */
  bool hasArt(const std::string& key) const;

 private:
  std::map<std::string, std::string> art_;
};

#endif  // ASCIIARTLIBRARY_HPP_INCLUDED
