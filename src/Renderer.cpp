#include "Renderer.hpp"

#include <string>
#include <vector>

namespace {

const char kRoadGlyph = '=';
const char kDoorGlyph = 'D';

/** Fills every cell with open ground, or road on road rows. */
void drawGround(Canvas& world, const SceneMap& map) {
  for (int y = 0; y < map.height(); ++y) {
    const char c = map.isRoad(y) ? kRoadGlyph : map.ground();
    for (int x = 0; x < map.width(); ++x) {
      world.put(x, y, c);
    }
  }
}

}  // namespace

Renderer::Renderer(const AsciiArtLibrary& art, Viewport& viewport)
    : art_(art), viewport_(viewport) {}

void Renderer::drawBuilding(Canvas& world, const Building& building) const {
  const Rect& r = building.footprint;
  if (art_.hasArt(building.id) && !art_.getArt(building.id).empty()) {
    Canvas sprite(r.width, r.height);
    sprite.blit(0, 0, art_.getArt(building.id));
    world.blit(r.x, r.y, sprite.toString());
  } else {
    for (int y = r.y; y < r.y + r.height; ++y) {
      for (int x = r.x; x < r.x + r.width; ++x) {
        world.put(x, y, ' ');
      }
    }
    world.drawBox(r);
    const int inner = r.width - 2;
    const std::string label = building.label.substr(
        0, inner > 0 ? static_cast<std::string::size_type>(inner) : 0);
    const int labelX = r.x + 1 + (inner - static_cast<int>(label.size())) / 2;
    world.drawText(labelX, r.y + r.height / 2, label);
  }
  world.put(building.door.x, building.door.y, kDoorGlyph);
}

std::string Renderer::renderFrame(const SceneMap& map,
                                  const std::vector<Entity>& entities,
                                  const Point& focus,
                                  const std::string& status) {
  Canvas world(map.width(), map.height());
  drawGround(world, map);
  for (std::vector<Building>::size_type i = 0; i < map.buildings().size();
       ++i) {
    drawBuilding(world, map.buildings()[i]);
  }
  for (std::vector<Entity>::size_type i = 0; i < entities.size(); ++i) {
    world.put(entities[i].pos.x, entities[i].pos.y, entities[i].glyph);
  }

  viewport_.follow(focus, map.width(), map.height());
  const Point origin = viewport_.origin();
  Canvas frame(viewport_.width(), viewport_.height() + 1);
  for (int y = 0; y < viewport_.height(); ++y) {
    for (int x = 0; x < viewport_.width(); ++x) {
      frame.put(x, y, world.get(origin.x + x, origin.y + y));
    }
  }
  frame.drawText(0, viewport_.height(), status);
  return frame.toString();
}
