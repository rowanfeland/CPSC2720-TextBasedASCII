#include "SceneLibrary.hpp"

#include <dirent.h>

#include <string>

#include "GreyImage.hpp"
#include "Pgm.hpp"
#include "SceneKind.hpp"

namespace {

const int kSourceWidth = 320;
const int kSourceHeight = 80;

bool endsWith(const std::string& s, const std::string& suffix) {
  return s.size() > suffix.size() &&
         s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;
}

}  // namespace

GreyImage makePlaceholder(SceneKind kind) {
  GreyImage img(kSceneWidth, kSceneHeight);
  const int horizon = kSceneHeight * 3 / 5;
  switch (kind) {
    case SceneKind::Outdoor:
      for (int y = 0; y < kSceneHeight; ++y) {
        const int v = y < horizon ? 230 - y * 3 : 80 - (y - horizon) * 2;
        img.fillRect(Rect(0, y, kSceneWidth, 1), v);
      }
      img.fillRect(Rect(60, horizon - 12, 40, 12), 40);
      img.fillRect(Rect(55, horizon - 14, 50, 2), 40);
      break;
    case SceneKind::Indoor:
      img.fillRect(Rect(0, 0, kSceneWidth, horizon), 150);
      img.fillRect(Rect(0, horizon, kSceneWidth, kSceneHeight - horizon), 70);
      img.fillRect(Rect(70, horizon - 20, 20, 20), 210);
      break;
    case SceneKind::Dark:
      img.fillRect(Rect(0, 0, kSceneWidth, kSceneHeight), 6);
      break;
  }
  return img;
}

bool SceneLibrary::loadFromDirectory(const std::string& path) {
  DIR* dir = opendir(path.c_str());
  if (dir == nullptr) {
    return false;
  }
  const std::string extension = ".pgm";
  for (const dirent* entry = readdir(dir); entry != nullptr;
       entry = readdir(dir)) {
    const std::string name(entry->d_name);
    if (!endsWith(name, extension)) {
      continue;
    }
    GreyImage img(1, 1);
    if (!loadPgm(path + "/" + name, img)) {
      continue;
    }
    if (img.width() == kSourceWidth && img.height() == kSourceHeight) {
      img = img.halved();
    }
    scenes_[name.substr(0, name.size() - extension.size())] = img;
  }
  closedir(dir);
  return true;
}

bool SceneLibrary::has(const std::string& key) const {
  return scenes_.find(key) != scenes_.end();
}

GreyImage SceneLibrary::sceneFor(const std::string& key, SceneKind kind) const {
  std::map<std::string, GreyImage>::const_iterator it = scenes_.find(key);
  return it == scenes_.end() ? makePlaceholder(kind) : it->second;
}
