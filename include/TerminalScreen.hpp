/**
 * @file TerminalScreen.hpp
 * @brief IScreen that writes to the real terminal (stdout).
 */
#ifndef TERMINALSCREEN_HPP_INCLUDED
#define TERMINALSCREEN_HPP_INCLUDED

#include <string>

#include "IScreen.hpp"

/** @brief Writes to stdout and reads the window size from the terminal. */
class TerminalScreen : public IScreen {
 public:
  /** @brief Writes the text to stdout and flushes it. */
  void write(const std::string& text) override;

  /** @brief Asks the terminal for its size. */
  bool size(int& cols, int& rows) override;
};

#endif  // TERMINALSCREEN_HPP_INCLUDED
