#include "Commands.hpp"

#include <memory>
#include <string>

#include "Command.hpp"
#include "CommandRegistry.hpp"
#include "Item.hpp"
#include "Room.hpp"

namespace {

std::string prefixed(const std::string& text, const std::string& ending) {
  return text + ending;
}

}  // namespace

std::string GoCommand::execute(const ParsedCommand& cmd, Player& player, GameState& state) {
  (void)state;
  if (cmd.noun.empty()) {
    return "Go where?";
  }
  Room* here = player.getCurrentRoom();
  Room* next = here->getExit(cmd.noun);
  if (next == nullptr) {
    return here->noExitMessage(cmd.noun);
  }
  std::string why;
  if (!next->canEnter(why)) {
    return why;
  }
  player.moveTo(next);
  return "";
}

std::string GoCommand::getHelp() const {
  return "go <exit>  (or n, s, e, w, enter <place>, leave)";
}

std::string LookCommand::execute(const ParsedCommand& cmd, Player& player, GameState& state) {
  (void)state;
  Room* here = player.getCurrentRoom();
  if (cmd.noun.empty()) {
    return here->describe();
  }
  if (here->itemsVisible()) {
    const Item* item = here->findItem(cmd.noun);
    if (item != nullptr) {
      return item->examine();
    }
  }
  const Item* carried = player.getInventory().find(cmd.noun);
  if (carried != nullptr) {
    return carried->examine();
  }
  return "You see no " + cmd.noun + " here.";
}

std::string LookCommand::getHelp() const {
  return "look [thing]  (look around, or examine something)";
}

std::string TakeCommand::execute(const ParsedCommand& cmd, Player& player, GameState& state) {
  (void)state;
  if (cmd.noun.empty()) {
    return "Take what?";
  }
  Room* here = player.getCurrentRoom();
  if (!here->itemsVisible()) {
    return "It's too dark to find anything.";
  }
  if (here->findItem(cmd.noun) == nullptr) {
    return "There is no " + cmd.noun + " here.";
  }
  if (player.getInventory().isFull()) {
    return "You can't carry any more.";
  }
  std::unique_ptr<Item> item = here->removeItem(cmd.noun);
  const std::string name = item->getName();
  player.getInventory().add(std::move(item));
  return "Taken: " + name + ".";
}

std::string TakeCommand::getHelp() const {
  return "take <item>  (pick something up)";
}

std::string DropCommand::execute(const ParsedCommand& cmd, Player& player, GameState& state) {
  (void)state;
  if (cmd.noun.empty()) {
    return "Drop what?";
  }
  std::unique_ptr<Item> item = player.getInventory().remove(cmd.noun);
  if (item == nullptr) {
    return "You aren't carrying " + cmd.noun + ".";
  }
  const std::string name = item->getName();
  player.getCurrentRoom()->addItem(std::move(item));
  return "Dropped: " + name + ".";
}

std::string DropCommand::getHelp() const {
  return "drop <item>  (put something down)";
}

std::string InventoryCommand::execute(const ParsedCommand& cmd, Player& player,
                                      GameState& state) {
  (void)cmd;
  return prefixed(player.getInventory().list(), "\nDay " + std::to_string(state.getDay()) + ".");
}

std::string InventoryCommand::getHelp() const {
  return "inventory  (check what you carry and the day)";
}

HelpCommand::HelpCommand(const CommandRegistry& registry) : registry_(registry) {}

std::string HelpCommand::execute(const ParsedCommand& cmd, Player& player, GameState& state) {
  (void)cmd;
  (void)player;
  (void)state;
  return registry_.helpText();
}

std::string HelpCommand::getHelp() const { return "help  (list all commands)"; }

std::string QuitCommand::execute(const ParsedCommand& cmd, Player& player, GameState& state) {
  (void)cmd;
  (void)player;
  state.requestQuit();
  return "You turn your horse around.";
}

std::string QuitCommand::getHelp() const { return "quit  (give up and leave)"; }
