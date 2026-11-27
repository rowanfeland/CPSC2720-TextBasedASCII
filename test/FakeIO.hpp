/**
 * @file FakeIO.hpp
 * @brief Test doubles for the display and input interfaces.
 */
#ifndef FAKEIO_HPP_INCLUDED
#define FAKEIO_HPP_INCLUDED

#include <string>
#include <vector>

#include "IDisplay.hpp"
#include "IInput.hpp"

/** @brief Records everything the game shows so tests can check it. */
class FakeDisplay : public IDisplay {
 public:
  FakeDisplay() : clears(0) {}

  void show(const std::string& text) override { output.push_back(text); }
  void showArt(const std::string& art) override { output.push_back(art); }
  void prompt(const std::string& text) override { output.push_back(text); }
  void clear() override { ++clears; }

  /** @brief Everything shown so far, joined into one string. */
  std::string all() const {
    std::string out;
    for (std::vector<std::string>::size_type i = 0; i < output.size(); ++i) {
      out += output[i] + "\n";
    }
    return out;
  }

  std::vector<std::string> output;
  int clears;
};

/** @brief Plays back a fixed list of lines; reports "quit" once they run out. */
class FakeInput : public IInput {
 public:
  explicit FakeInput(const std::vector<std::string>& lines) : lines_(lines), next_(0) {}

  std::string readLine() override {
    if (next_ >= lines_.size()) {
      return "quit";
    }
    return lines_[next_++];
  }

 private:
  std::vector<std::string> lines_;
  std::vector<std::string>::size_type next_;
};

#endif  // FAKEIO_HPP_INCLUDED
