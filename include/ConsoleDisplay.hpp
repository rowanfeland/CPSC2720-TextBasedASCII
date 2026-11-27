/**
 * @file ConsoleDisplay.hpp
 * @brief IDisplay that writes to a terminal stream.
 */
#ifndef CONSOLEDISPLAY_HPP_INCLUDED
#define CONSOLEDISPLAY_HPP_INCLUDED

#include <iostream>
#include <string>

#include "IDisplay.hpp"

/** @brief Writes game output to a stream (std::cout by default). */
class ConsoleDisplay : public IDisplay {
 public:
  /** @brief Creates a display writing to the given stream. */
  explicit ConsoleDisplay(std::ostream& out = std::cout);

  /** @brief Writes the text followed by a newline. */
  void show(const std::string& text) override;

  /** @brief Writes the art exactly as given. */
  void showArt(const std::string& art) override;

  /** @brief Writes the ANSI clear-screen and cursor-home codes. */
  void clear() override;

  /** @brief Writes the prompt and flushes, so the cursor waits on the same line. */
  void prompt(const std::string& text) override;

 private:
  std::ostream& out_;
};

#endif  // CONSOLEDISPLAY_HPP_INCLUDED
