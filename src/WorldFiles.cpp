#include "WorldFiles.hpp"

#include <dirent.h>

#include <algorithm>
#include <cctype>
#include <fstream>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

#include "Geometry.hpp"
#include "Rgb.hpp"
#include "WorldMap.hpp"

namespace {

const char kDefaultMap[] =
    "############\n"
    "#..........#\n"
    "#===@======#\n"
    "#..........#\n"
    "############\n";

std::vector<std::string> splitLines(const std::string& text) {
  std::vector<std::string> lines;
  std::istringstream in(text);
  std::string line;
  while (std::getline(in, line)) {
    if (!line.empty() && line[line.size() - 1] == '\r') {
      line.erase(line.size() - 1);
    }
    lines.push_back(line);
  }
  return lines;
}

bool isSkippable(const std::string& line) {
  const std::string::size_type first = line.find_first_not_of(" \t");
  return first == std::string::npos || line[first] == '#';
}

/** Splits on whitespace; "double quoted" text is one token. */
bool tokenize(const std::string& line, std::vector<std::string>& out) {
  std::string::size_type i = 0;
  while (i < line.size()) {
    if (std::isspace(static_cast<unsigned char>(line[i]))) {
      ++i;
    } else if (line[i] == '"') {
      const std::string::size_type close = line.find('"', i + 1);
      if (close == std::string::npos) {
        return false;
      }
      out.push_back(line.substr(i + 1, close - i - 1));
      i = close + 1;
    } else {
      std::string::size_type end = i;
      while (end < line.size() &&
             !std::isspace(static_cast<unsigned char>(line[end]))) {
        ++end;
      }
      out.push_back(line.substr(i, end - i));
      i = end;
    }
  }
  return true;
}

bool toInt(const std::string& s, int& out) {
  std::istringstream in(s);
  in >> out;
  return !in.fail() && in.eof();
}

bool toDouble(const std::string& s, double& out) {
  std::istringstream in(s);
  in >> out;
  return !in.fail() && in.eof();
}

bool toInts(const std::string& s, std::vector<int>& out, std::size_t count) {
  std::istringstream in(s);
  std::string part;
  out.clear();
  while (std::getline(in, part, ',')) {
    int v = 0;
    if (!toInt(part, v)) {
      return false;
    }
    out.push_back(v);
  }
  return out.size() == count;
}

bool toRgb(const std::string& s, Rgb& out) {
  std::vector<int> v;
  if (!toInts(s, v, 3)) {
    return false;
  }
  out = Rgb(v[0], v[1], v[2]);
  return true;
}

bool parseBuildingLine(const std::string& line, BuildingDef& b,
                       std::string& why) {
  std::vector<std::string> t;
  if (!tokenize(line, t)) {
    why = "unclosed quote";
    return false;
  }
  if (t.size() != 10) {
    why = "expected 10 fields";
    return false;
  }
  if (t[0].size() != 1 || t[0][0] < 'a' || t[0][0] > 'z') {
    why = "letter must be one lower-case letter";
    return false;
  }
  std::vector<int> door;
  if (!toDouble(t[3], b.height) || b.height <= 0) {
    why = "bad height";
    return false;
  }
  if (!toRgb(t[5], b.base) || !toRgb(t[6], b.trim)) {
    why = "colours must look like 150,88,62";
    return false;
  }
  if (!toInts(t[7], door, 2)) {
    why = "door must look like 6,15";
    return false;
  }
  if (t[8].size() != 1 || std::string("NSEW").find(t[8][0]) == std::string::npos) {
    why = "facing must be N, S, E or W";
    return false;
  }
  b.letter = t[0][0];
  b.id = t[1];
  b.label = t[2];
  b.style = t[4];
  b.doorX = door[0];
  b.doorY = door[1];
  b.facing = t[8][0];
  b.roomId = t[9];
  return true;
}

bool parseZoneLine(const std::string& line, Zone& z) {
  std::istringstream in(line);
  std::string id;
  std::string n[4];
  if (!(in >> id >> n[0] >> n[1] >> n[2] >> n[3])) {
    return false;
  }
  int v[4];
  for (int i = 0; i < 4; ++i) {
    if (!toInt(n[i], v[i])) {
      return false;
    }
  }
  std::string rest;
  std::getline(in, rest);
  const std::string::size_type first = rest.find_first_not_of(" \t");
  z.roomId = id;
  z.area = Rect(v[0], v[1], v[2] - v[0] + 1, v[3] - v[1] + 1);
  z.description = first == std::string::npos ? "" : rest.substr(first);
  return true;
}

bool parsePropLine(const std::string& line, Prop& p) {
  std::istringstream in(line);
  std::string name;
  std::string x;
  std::string y;
  std::string blocking;
  std::string extra;
  if (!(in >> name >> x >> y >> blocking) || (in >> extra)) {
    return false;
  }
  if (!toDouble(x, p.pos.x) || !toDouble(y, p.pos.y)) {
    return false;
  }
  if (blocking != "yes" && blocking != "no") {
    return false;
  }
  p.sprite = name;
  p.blocking = blocking == "yes";
  return true;
}

bool readFile(const std::string& path, std::string& out) {
  std::ifstream in(path.c_str(), std::ios::binary);
  if (!in) {
    return false;
  }
  std::ostringstream buffer;
  buffer << in.rdbuf();
  out = buffer.str();
  return true;
}

}  // namespace

bool parseTownMap(const std::string& text, WorldMap& map) {
  std::vector<std::string> rows = splitLines(text);
  while (!rows.empty() && rows.back().empty()) {
    rows.pop_back();
  }
  if (rows.empty()) {
    return false;
  }
  std::string::size_type width = 0;
  for (std::vector<std::string>::size_type i = 0; i < rows.size(); ++i) {
    width = std::max(width, rows[i].size());
  }
  bool found = false;
  Point2 firstOpen(0.5, 0.5);
  bool haveOpen = false;
  for (std::vector<std::string>::size_type y = 0; y < rows.size(); ++y) {
    rows[y].resize(width, '.');
    for (std::string::size_type x = 0; x < width; ++x) {
      const char c = rows[y][x];
      if (c == '@') {
        map.setStart(Point2(x + 0.5, y + 0.5));
        found = true;
      }
      const bool open = c != '#' && c != 'F' && !(c >= 'a' && c <= 'z');
      if (open && !haveOpen) {
        firstOpen = Point2(x + 0.5, y + 0.5);
        haveOpen = true;
      }
    }
  }
  map.setTiles(rows);
  if (!found) {
    map.setStart(firstOpen);
  }
  return true;
}

int parseBuildings(const std::string& text, WorldMap& map, std::ostream& warn) {
  const std::vector<std::string> lines = splitLines(text);
  int added = 0;
  for (std::vector<std::string>::size_type i = 0; i < lines.size(); ++i) {
    if (isSkippable(lines[i])) {
      continue;
    }
    BuildingDef b;
    std::string why;
    if (parseBuildingLine(lines[i], b, why)) {
      map.addBuilding(b);
      ++added;
    } else {
      warn << "buildings.txt line " << (i + 1) << " skipped: " << why << "\n";
    }
  }
  return added;
}

int parseZones(const std::string& text, WorldMap& map, std::ostream& warn) {
  const std::vector<std::string> lines = splitLines(text);
  int added = 0;
  for (std::vector<std::string>::size_type i = 0; i < lines.size(); ++i) {
    if (isSkippable(lines[i])) {
      continue;
    }
    Zone z;
    if (parseZoneLine(lines[i], z)) {
      map.addZone(z);
      ++added;
    } else {
      warn << "zones.txt line " << (i + 1)
           << " skipped: expected id x0 y0 x1 y1 description\n";
    }
  }
  return added;
}

int parseProps(const std::string& text, WorldMap& map, std::ostream& warn) {
  const std::vector<std::string> lines = splitLines(text);
  int added = 0;
  for (std::vector<std::string>::size_type i = 0; i < lines.size(); ++i) {
    if (isSkippable(lines[i])) {
      continue;
    }
    Prop p;
    if (parsePropLine(lines[i], p)) {
      map.addProp(p);
      ++added;
    } else {
      warn << "props.txt line " << (i + 1)
           << " skipped: expected sprite x y yes|no\n";
    }
  }
  return added;
}

namespace {

void loadSprites(const std::string& dir, WorldMap& map, std::ostream& warn) {
  DIR* folder = opendir(dir.c_str());
  if (folder == nullptr) {
    warn << "Missing sprite folder " << dir << "\n";
    return;
  }
  const std::string ext = ".txt";
  for (const dirent* e = readdir(folder); e != nullptr; e = readdir(folder)) {
    const std::string file(e->d_name);
    if (file.size() <= ext.size() ||
        file.compare(file.size() - ext.size(), ext.size(), ext) != 0) {
      continue;
    }
    std::string text;
    Sprite sprite;
    std::string why;
    if (readFile(dir + "/" + file, text) && parseSprite(text, sprite, why)) {
      map.addSprite(file.substr(0, file.size() - ext.size()), sprite);
    } else {
      warn << "sprites/" << file << " skipped: " << why << "\n";
    }
  }
  closedir(folder);
}

}  // namespace

void validateWorld(WorldMap& map, std::ostream& warn) {
  const std::vector<BuildingDef>& buildings = map.buildings();
  for (std::vector<BuildingDef>::size_type i = 0; i < buildings.size(); ++i) {
    const BuildingDef& b = buildings[i];
    if (map.isBlocked(b.doorX + 0.5, b.doorY + 0.5)) {
      warn << "buildings.txt: the door of " << b.id << " (" << b.doorX
           << "," << b.doorY
           << ") is on a blocked tile, so it can't be entered.\n";
      map.disableDoor(i);
    }
  }
}

WorldMap loadWorld(const std::string& dir, std::ostream& warn) {
  WorldMap map;
  std::string text;
  if (!readFile(dir + "/town.map", text) || !parseTownMap(text, map)) {
    warn << "World files not found in " << dir
         << "; using a small built-in street.\n";
    return defaultWorld();
  }
  if (readFile(dir + "/buildings.txt", text)) {
    parseBuildings(text, map, warn);
  } else {
    warn << "Missing " << dir << "/buildings.txt\n";
  }
  if (readFile(dir + "/zones.txt", text)) {
    parseZones(text, map, warn);
  } else {
    warn << "Missing " << dir << "/zones.txt\n";
  }
  if (readFile(dir + "/props.txt", text)) {
    parseProps(text, map, warn);
  } else {
    warn << "Missing " << dir << "/props.txt\n";
  }
  loadSprites(dir + "/sprites", map, warn);
  validateWorld(map, warn);
  if (map.isBlocked(map.start().x, map.start().y)) {
    warn << "The start in " << dir
         << "/town.map is blocked; using a small built-in street.\n";
    return defaultWorld();
  }
  SkyModel sky;
  if (sky.load(dir + "/sky.txt", warn)) {
    map.setSky(sky);
  } else {
    warn << "Missing " << dir << "/sky.txt\n";
  }
  return map;
}

WorldMap defaultWorld() {
  WorldMap map;
  parseTownMap(kDefaultMap, map);
  Zone street;
  street.roomId = "main_street";
  street.area = Rect(0, 0, map.width(), map.height());
  street.description = "A short dusty street.";
  map.addZone(street);
  return map;
}
