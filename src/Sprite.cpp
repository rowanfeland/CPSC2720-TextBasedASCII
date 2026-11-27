#include "Sprite.hpp"

#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "Rgb.hpp"

namespace {

bool readNumber(std::istream& in, double& out) {
  std::string token;
  if (!(in >> token)) {
    return false;
  }
  std::istringstream number(token);
  number >> out;
  return !number.fail() && number.eof();
}

}  // namespace

Sprite::Sprite() : width(0), height(0) {}

int Sprite::columns() const {
  return rows.empty() ? 0 : static_cast<int>(rows[0].size());
}

int Sprite::rowCount() const { return static_cast<int>(rows.size()); }

bool Sprite::pixel(int col, int row, Rgb& out) const {
  if (row < 0 || row >= rowCount() || col < 0 || col >= columns()) {
    return false;
  }
  const std::map<char, Rgb>::const_iterator it = palette.find(rows[row][col]);
  if (it == palette.end()) {
    return false;
  }
  out = it->second;
  return true;
}

bool parseSprite(const std::string& text, Sprite& out, std::string& why) {
  Sprite s;
  bool haveSize = false;
  std::istringstream in(text);
  std::string line;
  std::string::size_type widest = 0;
  while (std::getline(in, line)) {
    if (!line.empty() && line[line.size() - 1] == '\r') {
      line.erase(line.size() - 1);
    }
    const std::string::size_type first = line.find_first_not_of(" \t");
    if (first == std::string::npos || line[first] == '#') {
      continue;
    }
    if (line.compare(first, 5, "size ") == 0) {
      std::istringstream size(line.substr(first + 5));
      if (!readNumber(size, s.width) || !readNumber(size, s.height) ||
          s.width <= 0 || s.height <= 0) {
        why = "size line must look like: size 2.0 1.25";
        return false;
      }
      haveSize = true;
    } else if (line.find('=') != std::string::npos) {
      const std::string::size_type eq = line.find('=');
      std::istringstream rgb(line.substr(eq + 1));
      double r = 0;
      double g = 0;
      double b = 0;
      if (!readNumber(rgb, r) || !readNumber(rgb, g) || !readNumber(rgb, b)) {
        why = "colour line must look like: B = 122 74 40";
        return false;
      }
      s.palette[line[first]] = Rgb(static_cast<int>(r), static_cast<int>(g),
                                   static_cast<int>(b));
    } else {
      const std::string row = line.substr(first);
      widest = std::max(widest, row.size());
      s.rows.push_back(row);
    }
  }
  if (!haveSize) {
    why = "missing size line";
    return false;
  }
  if (s.rows.empty()) {
    why = "no pixel rows";
    return false;
  }
  for (std::vector<std::string>::size_type i = 0; i < s.rows.size(); ++i) {
    s.rows[i].resize(widest, '.');
  }
  out = s;
  return true;
}
