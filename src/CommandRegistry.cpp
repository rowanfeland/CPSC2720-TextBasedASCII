#include "CommandRegistry.hpp"

#include <map>
#include <memory>
#include <string>

#include "Command.hpp"
#include "Commands.hpp"
#include "TextUtil.hpp"

CommandRegistry::CommandRegistry() {}

CommandRegistry::~CommandRegistry() {}

void CommandRegistry::registerCommand(const std::string& verb,
                                      std::unique_ptr<Command> cmd) {
  commands_[verb] = std::move(cmd);
}

void CommandRegistry::addAlias(const std::string& alias,
                               const std::string& expansion) {
  aliases_[normalize(alias)] = normalize(expansion);
}

std::string CommandRegistry::expand(const std::string& line) const {
  const std::string n = normalize(line);
  const std::string::size_type space = n.find(' ');
  const std::string first = space == std::string::npos ? n : n.substr(0, space);
  const std::map<std::string, std::string>::const_iterator it = aliases_.find(first);
  if (it == aliases_.end()) {
    return n;
  }
  return space == std::string::npos ? it->second
                                    : it->second + n.substr(space);
}

Command* CommandRegistry::find(const std::string& verb) const {
  const std::map<std::string, std::unique_ptr<Command>>::const_iterator it =
      commands_.find(verb);
  return it == commands_.end() ? nullptr : it->second.get();
}

std::string CommandRegistry::helpText() const {
  std::string out;
  for (std::map<std::string, std::unique_ptr<Command>>::const_iterator it =
           commands_.begin();
       it != commands_.end(); ++it) {
    out += it->second->getHelp() + "\n";
  }
  return out;
}

void CommandRegistry::registerDefaults(CommandRegistry& registry) {
  registry.registerCommand("go", std::unique_ptr<Command>(new GoCommand()));
  registry.registerCommand("look", std::unique_ptr<Command>(new LookCommand()));
  registry.registerCommand("take", std::unique_ptr<Command>(new TakeCommand()));
  registry.registerCommand("drop", std::unique_ptr<Command>(new DropCommand()));
  registry.registerCommand("inventory",
                           std::unique_ptr<Command>(new InventoryCommand()));
  registry.registerCommand("help",
                           std::unique_ptr<Command>(new HelpCommand(registry)));
  registry.registerCommand("quit", std::unique_ptr<Command>(new QuitCommand()));

  registry.addAlias("n", "go north");
  registry.addAlias("s", "go south");
  registry.addAlias("e", "go east");
  registry.addAlias("w", "go west");
  registry.addAlias("u", "go up");
  registry.addAlias("d", "go down");
  registry.addAlias("north", "go north");
  registry.addAlias("south", "go south");
  registry.addAlias("east", "go east");
  registry.addAlias("west", "go west");
  registry.addAlias("up", "go up");
  registry.addAlias("down", "go down");
  registry.addAlias("enter", "go");
  registry.addAlias("leave", "go out");
  registry.addAlias("exit", "go out");
  registry.addAlias("out", "go out");
  registry.addAlias("l", "look");
  registry.addAlias("get", "take");
  registry.addAlias("i", "inventory");
  registry.addAlias("inv", "inventory");
  registry.addAlias("q", "quit");
}
