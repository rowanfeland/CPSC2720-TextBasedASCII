/**
 * @file ConsoleInput.hpp
 * @brief IInput that reads lines from a terminal stream.
 */
#ifndef CONSOLEINPUT_HPP_INCLUDED
#define CONSOLEINPUT_HPP_INCLUDED

#include <iostream>
#include <string>

#include "IInput.hpp"

/** @brief Reads player commands from a stream (std::cin by default). */
class ConsoleInput : public IInput {
 public:
  /** @brief Creates an input reading from the given stream. */
  explicit ConsoleInput(std::istream& in = std::cin);

  /**
   * @brief Reads one line, without a trailing carriage return.
   * Returns "quit" at end of input so the game never loops forever.
   */
  std::string readLine() override;

 private:
  std::istream& in_;
};

#endif  // CONSOLEINPUT_HPP_INCLUDED
