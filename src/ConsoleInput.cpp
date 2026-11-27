#include "ConsoleInput.hpp"

#include <iostream>
#include <string>

ConsoleInput::ConsoleInput(std::istream& in) : in_(in) {}

std::string ConsoleInput::readLine() {
  std::string line;
  if (!std::getline(in_, line)) {
    return "quit";
  }
  if (!line.empty() && line[line.size() - 1] == '\r') {
    line.erase(line.size() - 1);
  }
  return line;
}
