/**
 * @file main.cpp
 * @brief Entry point for The Line: walk Main Street in placeholder boxes.
 */
#include <string>
#include <vector>

#include "AsciiArtLibrary.hpp"
#include "ConsoleDisplay.hpp"
#include "ConsoleInput.hpp"
#include "Entity.hpp"
#include "Movement.hpp"
#include "Renderer.hpp"
#include "SceneMap.hpp"
#include "TownLayout.hpp"
#include "Viewport.hpp"

namespace {

const int kViewWidth = 80;
const int kViewHeight = 13;
const char kPlayerGlyph = '@';
const char kArtFolder[] = "data/art";

std::string statusFor(const SceneMap& map, const Point& pos) {
  const Building* door = map.buildingAtDoor(pos);
  if (door != nullptr) {
    return door->label + " (closed for now)";
  }
  return map.name() + " | Day 1 | n/s/e/w to move, quit to leave";
}

}  // namespace

int main() {
  AsciiArtLibrary art;
  art.loadFromDirectory(kArtFolder);  // No art yet: buildings draw as boxes.

  const SceneMap map = TownLayout::buildMainStreet();
  Viewport viewport(kViewWidth, kViewHeight);
  Renderer renderer(art, viewport);
  ConsoleDisplay display;
  ConsoleInput input;

  Point player = TownLayout::startPosition();
  std::string message = "You ride into town from the east.";

  while (true) {
    std::vector<Entity> entities;
    entities.push_back(Entity("player", player, kPlayerGlyph));

    display.clear();
    display.showArt(
        renderer.renderFrame(map, entities, player, statusFor(map, player)));
    display.show(message);
    display.show("> ");

    const std::string line = input.readLine();
    if (line == "quit" || line == "q") {
      display.show("You turn your horse around.");
      break;
    }
    const Direction direction = parseDirection(line);
    if (direction == Direction::None) {
      message = "Unknown command. Try n, s, e, w or quit.";
    } else if (tryMove(map, player, direction)) {
      message = "";
    } else {
      message = "You can't go that way.";
    }
  }
  return 0;
}
