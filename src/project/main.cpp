/**
 * @file main.cpp
 * @brief Entry point for The Line.
 *
 * Usage (run from the repo root):  ./playGame [--plain]
 *   --plain  print density characters instead of grey pixels (no colour codes)
 */
#include <cstring>
#include <memory>

#include "CommandRegistry.hpp"
#include "ConsoleDisplay.hpp"
#include "ConsoleInput.hpp"
#include "Game.hpp"
#include "GameState.hpp"
#include "Player.hpp"
#include "SceneLibrary.hpp"
#include "SceneRenderer.hpp"
#include "World.hpp"
#include "WorldBuilder.hpp"

namespace {

const char kArtFolder[] = "data/art";
const int kCarryCapacity = 6;

}  // namespace

int main(int argc, char** argv) {
  bool plain = false;
  for (int i = 1; i < argc; ++i) {
    if (std::strcmp(argv[i], "--plain") == 0) {
      plain = true;
    }
  }

  SceneLibrary scenes;
  scenes.loadFromDirectory(kArtFolder);  // Missing art falls back to placeholders.

  std::unique_ptr<World> world = WorldBuilder::build();
  CommandRegistry registry;
  CommandRegistry::registerDefaults(registry);
  GameState state;
  Player player(world->getStartRoom(), kCarryCapacity);
  ConsoleDisplay display;
  ConsoleInput input;

  Game game(*world, player, state, registry, scenes, display, input,
            plain ? ColourMode::Plain : ColourMode::Grey);
  game.run();
  return 0;
}
