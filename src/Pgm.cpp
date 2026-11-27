#include "Pgm.hpp"

#include <cctype>
#include <fstream>
#include <sstream>
#include <string>

#include "GreyImage.hpp"

namespace {

/** Reads the next whitespace-separated token, skipping # comments. */
bool nextToken(std::istream& in, std::string& token) {
  while (in >> token) {
    if (token[0] != '#') {
      return true;
    }
    std::getline(in, token);  // discard rest of the comment line
  }
  return false;
}

bool nextInt(std::istream& in, int& value) {
  std::string token;
  if (!nextToken(in, token)) {
    return false;
  }
  for (std::string::size_type i = 0; i < token.size(); ++i) {
    if (!std::isdigit(static_cast<unsigned char>(token[i]))) {
      return false;
    }
  }
  std::istringstream digits(token);
  return static_cast<bool>(digits >> value);
}

}  // namespace

bool parsePgm(const std::string& data, GreyImage& out) {
  std::istringstream in(data);
  std::string magic;
  if (!nextToken(in, magic) || (magic != "P2" && magic != "P5")) {
    return false;
  }
  int width = 0;
  int height = 0;
  int maxValue = 0;
  if (!nextInt(in, width) || !nextInt(in, height) || !nextInt(in, maxValue)) {
    return false;
  }
  if (width <= 0 || height <= 0 || maxValue <= 0 || maxValue > 65535) {
    return false;
  }
  GreyImage img(width, height);
  const int count = width * height;
  if (magic == "P2") {
    for (int i = 0; i < count; ++i) {
      int v = 0;
      if (!nextInt(in, v)) {
        return false;
      }
      img.set(i % width, i / width, v * 255 / maxValue);
    }
  } else {
    in.get();  // single whitespace byte after maxval
    for (int i = 0; i < count; ++i) {
      const int byte = in.get();
      if (byte == std::char_traits<char>::eof()) {
        return false;
      }
      img.set(i % width, i / width, byte * 255 / maxValue);
    }
  }
  out = img;
  return true;
}

bool loadPgm(const std::string& path, GreyImage& out) {
  std::ifstream file(path.c_str(), std::ios::binary);
  if (!file) {
    return false;
  }
  std::ostringstream buffer;
  buffer << file.rdbuf();
  return parsePgm(buffer.str(), out);
}
