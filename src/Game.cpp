#include "Game.hpp"

#include <cctype>
#include <string>

#include "Command.hpp"
#include "CommandParser.hpp"
#include "CommandRegistry.hpp"
#include "GameState.hpp"
#include "IDisplay.hpp"
#include "IInput.hpp"
#include "Player.hpp"
#include "Room.hpp"
#include "SceneLibrary.hpp"
#include "SceneRenderer.hpp"
#include "World.hpp"

namespace {

std::string upper(const std::string& s) {
  std::string out = s;
  for (std::string::size_type i = 0; i < out.size(); ++i) {
    out[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(out[i])));
  }
  return out;
}

}  // namespace

Game::Game(World& world, Player& player, GameState& state, CommandRegistry& registry,
           const SceneLibrary& scenes, IDisplay& display, IInput& input, ColourMode mode)
    : world_(world),
      player_(player),
      state_(state),
      registry_(registry),
      scenes_(scenes),
      display_(display),
      input_(input),
      mode_(mode) {}

void Game::run() {
  showScreen();
  while (true) {
    const std::string line = input_.readLine();
    if (!processTurn(line)) {
      display_.show(lastMessage_);
      break;
    }
    showScreen();
  }
}

bool Game::processTurn(const std::string& line) {
  const ParsedCommand parsed = CommandParser::parse(registry_.expand(line));
  if (parsed.isEmpty()) {
    lastMessage_ = "Type a command, or help.";
  } else {
    Command* cmd = registry_.find(parsed.verb);
    if (cmd == nullptr) {
      lastMessage_ = "I don't understand \"" + line + "\". Type help.";
    } else {
      lastMessage_ = cmd->execute(parsed, player_, state_);
    }
  }
  return !state_.quitRequested();
}

const std::string& Game::lastMessage() const { return lastMessage_; }

void Game::showScreen() {
  Room* room = player_.getCurrentRoom();
  display_.clear();
  display_.showArt(renderScene(scenes_.sceneFor(room->getId(), room->sceneKind()),
                               state_.getDay(), mode_));
  display_.show(upper(room->getName()));
  display_.show(room->describe());
  if (!lastMessage_.empty()) {
    display_.show(lastMessage_);
  }
  display_.prompt("> ");
}
