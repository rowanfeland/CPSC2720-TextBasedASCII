#include "CommandParser.hpp"

#include <string>

#include "TextUtil.hpp"

bool ParsedCommand::isEmpty() const { return verb.empty(); }

ParsedCommand CommandParser::parse(const std::string& line) {
  ParsedCommand out;
  const std::string n = normalize(line);
  const std::string::size_type space = n.find(' ');
  if (space == std::string::npos) {
    out.verb = n;
    return out;
  }
  out.verb = n.substr(0, space);
  const std::string rest = n.substr(space + 1);

  const std::string separators[] = {" to ", " on "};
  for (std::string::size_type i = 0; i < 2; ++i) {
    const std::string::size_type at = rest.find(separators[i]);
    if (at != std::string::npos) {
      out.noun = rest.substr(0, at);
      out.target = rest.substr(at + separators[i].size());
      return out;
    }
  }
  out.noun = rest;
  return out;
}
