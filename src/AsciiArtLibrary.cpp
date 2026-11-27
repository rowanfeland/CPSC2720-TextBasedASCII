#include "AsciiArtLibrary.hpp"

#include <dirent.h>

#include <fstream>
#include <sstream>
#include <string>

namespace {

const char kArtExtension[] = ".txt";

bool endsWith(const std::string& s, const std::string& suffix) {
  return s.size() > suffix.size() &&
         s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::string readWithoutCarriageReturns(const std::string& filePath) {
  std::ifstream in(filePath.c_str());
  std::ostringstream buffer;
  buffer << in.rdbuf();
  std::string text = buffer.str();
  std::string cleaned;
  cleaned.reserve(text.size());
  for (std::string::size_type i = 0; i < text.size(); ++i) {
    if (text[i] != '\r') {
      cleaned += text[i];
    }
  }
  return cleaned;
}

}  // namespace

bool AsciiArtLibrary::loadFromDirectory(const std::string& path) {
  DIR* dir = opendir(path.c_str());
  if (dir == nullptr) {
    return false;
  }
  const std::string extension(kArtExtension);
  for (const dirent* entry = readdir(dir); entry != nullptr;
       entry = readdir(dir)) {
    const std::string name(entry->d_name);
    if (endsWith(name, extension)) {
      const std::string key = name.substr(0, name.size() - extension.size());
      art_[key] = readWithoutCarriageReturns(path + "/" + name);
    }
  }
  closedir(dir);
  return true;
}

void AsciiArtLibrary::addArt(const std::string& key, const std::string& art) {
  art_[key] = art;
}

std::string AsciiArtLibrary::getArt(const std::string& key) const {
  std::map<std::string, std::string>::const_iterator it = art_.find(key);
  return it == art_.end() ? std::string() : it->second;
}

bool AsciiArtLibrary::hasArt(const std::string& key) const {
  return art_.find(key) != art_.end();
}
