#include "ConsoleDisplay.hpp"

#include <iostream>
#include <string>

namespace {

const char kClearScreen[] = "\033[2J\033[H";

}  // namespace

ConsoleDisplay::ConsoleDisplay(std::ostream& out) : out_(out) {}

void ConsoleDisplay::show(const std::string& text) { out_ << text << '\n'; }

void ConsoleDisplay::showArt(const std::string& art) { out_ << art; }

void ConsoleDisplay::clear() { out_ << kClearScreen; }
