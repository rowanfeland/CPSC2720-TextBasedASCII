/**
 * @file Command.hpp
 * @brief Base for every player command.
 */
#ifndef COMMAND_HPP_INCLUDED
#define COMMAND_HPP_INCLUDED

#include <string>

#include "CommandParser.hpp"
#include "GameState.hpp"
#include "Player.hpp"

/** @brief A player action. Subclasses implement one verb each. */
class Command {
 public:
  /** @brief Virtual destructor. */
  virtual ~Command();

  /**
   * @brief Carries out the command.
   * @return The text to show the player (may be empty).
   */
  virtual std::string execute(const ParsedCommand& cmd, Player& player,
                              GameState& state) = 0;

  /** @brief One-line usage hint for the help screen. */
  virtual std::string getHelp() const = 0;
};

#endif  // COMMAND_HPP_INCLUDED
