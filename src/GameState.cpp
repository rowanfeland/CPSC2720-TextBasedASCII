#include "GameState.hpp"

#include <string>

GameState::GameState() : day_(1), quit_(false) {}

int GameState::getDay() const { return day_; }

void GameState::setDay(int day) { day_ = day; }

void GameState::setFlag(const std::string& name) { flags_.insert(name); }

bool GameState::hasFlag(const std::string& name) const {
  return flags_.count(name) > 0;
}

void GameState::requestQuit() { quit_ = true; }

bool GameState::quitRequested() const { return quit_; }
