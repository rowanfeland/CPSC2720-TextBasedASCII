/**
 * @file CommandParser.hpp
 * @brief Splits a typed line into verb, noun and target.
 */
#ifndef COMMANDPARSER_HPP_INCLUDED
#define COMMANDPARSER_HPP_INCLUDED

#include <string>

/** @brief A typed command split into its parts. Empty strings where absent. */
struct ParsedCommand {
  std::string verb;    ///< First word, e.g. "take".
  std::string noun;    ///< Words after the verb, e.g. "tin cup".
  std::string target;  ///< Words after " to " or " on ", e.g. "horse".

  /** @brief True when nothing was typed. */
  bool isEmpty() const;
};

/** @brief Turns a typed line into a ParsedCommand. Ignores case and extra spaces. */
class CommandParser {
 public:
  /** @brief Parses one line of player input. */
  static ParsedCommand parse(const std::string& line);
};

#endif  // COMMANDPARSER_HPP_INCLUDED
