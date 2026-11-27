/**
 * @file IDisplay.hpp
 * @brief Output interface, so game logic can be tested without a terminal.
 */
#ifndef IDISPLAY_HPP_INCLUDED
#define IDISPLAY_HPP_INCLUDED

#include <string>

/** @brief Where the game sends text and frames. */
class IDisplay {
 public:
  virtual ~IDisplay() {}

  /** @brief Outputs one line of game text. */
  virtual void show(const std::string& text) = 0;

  /** @brief Outputs a block of ASCII art exactly as written. */
  virtual void showArt(const std::string& art) = 0;

  /** @brief Clears the screen before the next frame. */
  virtual void clear() = 0;

  /** @brief Shows the input prompt on the current line, without a newline. */
  virtual void prompt(const std::string& text) = 0;
};

#endif  // IDISPLAY_HPP_INCLUDED
