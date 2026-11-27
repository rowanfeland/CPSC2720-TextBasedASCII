/**
 * @file IScreen.hpp
 * @brief Where the 3D view is written, so the game can be tested without a
 * real terminal.
 */
#ifndef ISCREEN_HPP_INCLUDED
#define ISCREEN_HPP_INCLUDED

#include <string>

/** @brief A terminal-like output with a known size. */
class IScreen {
 public:
  virtual ~IScreen() {}

  /** @brief Writes text (including escape codes) and shows it at once. */
  virtual void write(const std::string& text) = 0;

  /** @brief Current size in character cells; false if unknown. */
  virtual bool size(int& cols, int& rows) = 0;
};

#endif  // ISCREEN_HPP_INCLUDED
