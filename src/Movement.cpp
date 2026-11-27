#include "Movement.hpp"

#include <cctype>
#include <string>

namespace {

std::string normalize(const std::string& word) {
  const std::string whitespace = " \t\r\n";
  const std::string::size_type start = word.find_first_not_of(whitespace);
  if (start == std::string::npos) {
    return "";
  }
  const std::string::size_type end = word.find_last_not_of(whitespace);
  std::string out = word.substr(start, end - start + 1);
  for (std::string::size_type i = 0; i < out.size(); ++i) {
    out[i] = static_cast<char>(
        std::tolower(static_cast<unsigned char>(out[i])));
  }
  return out;
}

}  // namespace

Direction parseDirection(const std::string& word) {
  const std::string w = normalize(word);
  if (w == "n" || w == "north") {
    return Direction::North;
  }
  if (w == "s" || w == "south") {
    return Direction::South;
  }
  if (w == "e" || w == "east") {
    return Direction::East;
  }
  if (w == "w" || w == "west") {
    return Direction::West;
  }
  return Direction::None;
}

Point step(const Point& from, Direction direction) {
  switch (direction) {
    case Direction::North:
      return Point(from.x, from.y - 1);
    case Direction::South:
      return Point(from.x, from.y + 1);
    case Direction::East:
      return Point(from.x + 1, from.y);
    case Direction::West:
      return Point(from.x - 1, from.y);
    default:
      return from;
  }
}

bool tryMove(const SceneMap& map, Point& position, Direction direction) {
  if (direction == Direction::None) {
    return false;
  }
  const Point next = step(position, direction);
  if (!map.isWalkable(next)) {
    return false;
  }
  position = next;
  return true;
}
