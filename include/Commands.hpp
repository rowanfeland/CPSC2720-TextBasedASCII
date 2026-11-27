/**
 * @file Commands.hpp
 * @brief The seven built-in player commands.
 */
#ifndef COMMANDS_HPP_INCLUDED
#define COMMANDS_HPP_INCLUDED

#include <string>

#include "Command.hpp"
#include "CommandRegistry.hpp"

/** @brief "go <exit>": moves through a named exit if the room allows it. */
class GoCommand : public Command {
 public:
  std::string execute(const ParsedCommand& cmd, Player& player, GameState& state) override;
  std::string getHelp() const override;
};

/** @brief "look" or "look <thing>": describes the room or examines an item. */
class LookCommand : public Command {
 public:
  std::string execute(const ParsedCommand& cmd, Player& player, GameState& state) override;
  std::string getHelp() const override;
};

/** @brief "take <item>": picks up an item from the room. */
class TakeCommand : public Command {
 public:
  std::string execute(const ParsedCommand& cmd, Player& player, GameState& state) override;
  std::string getHelp() const override;
};

/** @brief "drop <item>": puts an item from the inventory on the floor. */
class DropCommand : public Command {
 public:
  std::string execute(const ParsedCommand& cmd, Player& player, GameState& state) override;
  std::string getHelp() const override;
};

/** @brief "inventory": lists what is carried, and the day. */
class InventoryCommand : public Command {
 public:
  std::string execute(const ParsedCommand& cmd, Player& player, GameState& state) override;
  std::string getHelp() const override;
};

/** @brief "help": lists every command. Holds the registry it describes. */
class HelpCommand : public Command {
 public:
  /** @param registry Registry whose commands are listed; must outlive this command. */
  explicit HelpCommand(const CommandRegistry& registry);
  std::string execute(const ParsedCommand& cmd, Player& player, GameState& state) override;
  std::string getHelp() const override;

 private:
  const CommandRegistry& registry_;
};

/** @brief "quit": asks the game to end. */
class QuitCommand : public Command {
 public:
  std::string execute(const ParsedCommand& cmd, Player& player, GameState& state) override;
  std::string getHelp() const override;
};

#endif  // COMMANDS_HPP_INCLUDED
