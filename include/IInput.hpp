/**
 * @file IInput.hpp
 * @brief Input interface, so tests can feed scripted commands.
 */
#ifndef IINPUT_HPP_INCLUDED
#define IINPUT_HPP_INCLUDED

#include <string>

/** @brief Where the game reads player commands from. */
class IInput {
 public:
  virtual ~IInput() {}

  /** @brief Returns the next line the player typed. */
  virtual std::string readLine() = 0;
};

#endif  // IINPUT_HPP_INCLUDED
