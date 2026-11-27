#include "TextUtil.hpp"

#include <cctype>
#include <string>
#include <vector>

std::string normalize(const std::string& s) {
  std::string out;
  bool pendingSpace = false;
  for (std::string::size_type i = 0; i < s.size(); ++i) {
    const unsigned char c = static_cast<unsigned char>(s[i]);
    if (std::isspace(c)) {
      pendingSpace = !out.empty();
    } else {
      if (pendingSpace) {
        out += ' ';
        pendingSpace = false;
      }
      out += static_cast<char>(std::tolower(c));
    }
  }
  return out;
}

std::string joinList(const std::vector<std::string>& items) {
  std::string out;
  for (std::vector<std::string>::size_type i = 0; i < items.size(); ++i) {
    if (i > 0) {
      out += (i + 1 == items.size()) ? " and " : ", ";
    }
    out += items[i];
  }
  return out;
}
