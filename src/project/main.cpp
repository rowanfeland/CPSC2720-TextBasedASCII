/**
 * @file main.cpp
 * @brief Entry point for The Line.
 *
 * Usage (run from the repo root):  ./playGame [--plain]
 *   In a live terminal you walk the town in first-person 3D: arrow keys move,
 *   everything else is typed. With piped input (scripts, tests) the typed
 *   location game runs instead.
 *   --plain  draw with density characters instead of colour
 */
#include <cstring>
#include <exception>
#include <iostream>
#include <memory>

#include "CommandRegistry.hpp"
#include "ConsoleDisplay.hpp"
#include "ConsoleInput.hpp"
#include "ExteriorGame.hpp"
#include "Game.hpp"
#include "GameState.hpp"
#include "Player.hpp"
#include "RawTerminal.hpp"
#include "SceneLibrary.hpp"
#include "SceneRenderer.hpp"
#include "TerminalKeyInput.hpp"
#include "TerminalScreen.hpp"
#include "World.hpp"
#include "WorldBuilder.hpp"
#include "WorldFiles.hpp"

namespace {

const char kArtFolder[] = "data/art";
const char kWorldFolder[] = "data/world";
const int kCarryCapacity = 6;

}  // namespace

int main(int argc, char** argv) {
  bool plain = false;
  for (int i = 1; i < argc; ++i) {
    if (std::strcmp(argv[i], "--plain") == 0) {
      plain = true;
    }
  }

  std::unique_ptr<World> world = WorldBuilder::build();
  CommandRegistry registry;
  CommandRegistry::registerDefaults(registry);
  GameState state;
  Player player(world->getStartRoom(), kCarryCapacity);

  SceneLibrary scenes;
  scenes.loadFromDirectory(kArtFolder);  // Missing art falls back to placeholders.

  if (RawTerminal::isInteractive()) {
    const WorldMap map = loadWorld(kWorldFolder, std::cerr);
    RawTerminal raw;
    TerminalScreen screen;
    TerminalKeyInput keys;
    ExteriorGame game(map, *world, player, state, registry, keys, screen, plain);
    game.setScenes(&scenes);
    try {
      game.run();
    } catch (const std::exception& e) {
      RawTerminal::restore();  // never leave the terminal raw
      std::cerr << "The Line stopped: " << e.what() << "\n";
      return 1;
    } catch (...) {
      RawTerminal::restore();
      std::cerr << "The Line stopped unexpectedly.\n";
      return 1;
    }
    return 0;
  }

  ConsoleDisplay display;
  ConsoleInput input;
  Game game(*world, player, state, registry, scenes, display, input,
            plain ? ColourMode::Plain : ColourMode::Grey);
  game.run();
  return 0;
}
