/**
 * @file GameState.hpp
 * @brief Story progress shared by all commands: the day, flags and quit request.
 */
#ifndef GAMESTATE_HPP_INCLUDED
#define GAMESTATE_HPP_INCLUDED

#include <set>
#include <string>

/** @brief Mutable game-wide state. */
class GameState {
 public:
  /** @brief Starts on day 1 with no flags set. */
  GameState();

  /** @brief The current day, starting at 1. */
  int getDay() const;

  /** @brief Sets the current day. */
  void setDay(int day);

  /** @brief Records a story flag such as "marker_returned". */
  void setFlag(const std::string& name);

  /** @brief True if the flag has been set. */
  bool hasFlag(const std::string& name) const;

  /** @brief Asks the game loop to end. */
  void requestQuit();

  /** @brief True once a quit has been requested. */
  bool quitRequested() const;

 private:
  int day_;
  bool quit_;
  std::set<std::string> flags_;
};

#endif  // GAMESTATE_HPP_INCLUDED
