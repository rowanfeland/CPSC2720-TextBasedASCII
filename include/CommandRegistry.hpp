/**
 * @file CommandRegistry.hpp
 * @brief Maps verbs and shortcuts to commands, and builds the help text.
 */
#ifndef COMMANDREGISTRY_HPP_INCLUDED
#define COMMANDREGISTRY_HPP_INCLUDED

#include <map>
#include <memory>
#include <string>

#include "Command.hpp"

/** @brief Owns the commands and the shortcuts that expand into them. */
class CommandRegistry {
 public:
  /** @brief Creates an empty registry. */
  CommandRegistry();

  /** @brief Destroys every registered command. */
  ~CommandRegistry();

  /** @brief Registers a command under a verb. Takes ownership. */
  void registerCommand(const std::string& verb, std::unique_ptr<Command> cmd);

  /**
   * @brief Makes a shortcut: typing alias is the same as typing expansion.
   * Example: "n" expands to "go north".
   */
  void addAlias(const std::string& alias, const std::string& expansion);

  /**
   * @brief Replaces a leading alias in a line with its expansion.
   * Lines that start with no alias are returned normalized but unchanged.
   */
  std::string expand(const std::string& line) const;

  /** @brief The command for a verb, or nullptr if there is none. */
  Command* find(const std::string& verb) const;

  /** @brief One usage line per command, sorted by verb. */
  std::string helpText() const;

  /** @brief Registers the seven built-in commands and their shortcuts. */
  static void registerDefaults(CommandRegistry& registry);

 private:
  std::map<std::string, std::unique_ptr<Command>> commands_;
  std::map<std::string, std::string> aliases_;
};

#endif  // COMMANDREGISTRY_HPP_INCLUDED
