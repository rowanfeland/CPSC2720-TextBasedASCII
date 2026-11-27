#include "TerminalScreen.hpp"

#include <cstdio>
#include <string>

#include "RawTerminal.hpp"

void TerminalScreen::write(const std::string& text) {
  std::fwrite(text.data(), 1, text.size(), stdout);
  std::fflush(stdout);
}

bool TerminalScreen::size(int& cols, int& rows) {
  return RawTerminal::size(cols, rows);
}
