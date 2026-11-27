/**
 * @file SceneLibrary.hpp
 * @brief Loads scene pictures from data/art and makes placeholders for the rest.
 */
#ifndef SCENELIBRARY_HPP_INCLUDED
#define SCENELIBRARY_HPP_INCLUDED

#include <map>
#include <string>

#include "GreyImage.hpp"
#include "SceneKind.hpp"

/** @brief Width of a scene picture, in terminal cells. */
const int kSceneWidth = 160;
/** @brief Height of a scene picture, in terminal rows. */
const int kSceneHeight = 40;

/**
 * @brief Generated stand-in picture for a place with no drawing yet.
 * Outdoor: bright sky over darker ground with a building silhouette.
 * Indoor: mid-grey walls, darker floor, lighter door. Dark: nearly black.
 */
GreyImage makePlaceholder(SceneKind kind);

/** @brief Holds loaded scenes keyed by room id. */
class SceneLibrary {
 public:
  /**
   * @brief Loads every *.pgm in a folder, keyed by file name without extension.
   * Images 320x80 are halved. Files that fail to parse are skipped.
   * @return false if the folder cannot be opened.
   */
  bool loadFromDirectory(const std::string& path);

  /** @brief True if a picture was loaded for this key. */
  bool has(const std::string& key) const;

  /** @brief The loaded picture for the key, or a placeholder of the given kind. */
  GreyImage sceneFor(const std::string& key, SceneKind kind) const;

 private:
  std::map<std::string, GreyImage> scenes_;
};

#endif  // SCENELIBRARY_HPP_INCLUDED
