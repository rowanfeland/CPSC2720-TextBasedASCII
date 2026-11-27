/**
 * @file GameClock.hpp
 * @brief Time of day in the game, in minutes since midnight.
 */
#ifndef GAMECLOCK_HPP_INCLUDED
#define GAMECLOCK_HPP_INCLUDED

#include <string>

/** @brief Game start time: 17:30, riding in at sunset. */
const int kStartMinutes = 17 * 60 + 30;
/** @brief Minutes one step costs. */
const int kStepMinutes = 2;
/** @brief Minutes a successful typed action costs. */
const int kActionMinutes = 5;
/** @brief Minutes "wait" costs. */
const int kWaitMinutes = 60;

/** @brief A 24-hour clock that only moves forward. */
class GameClock {
 public:
  /** @brief Starts at the given minute of the day (wrapped to 0..1439). */
  explicit GameClock(int minutes = kStartMinutes);

  /** @brief Minutes since midnight, 0..1439. */
  int minutes() const;

  /**
   * @brief Moves time forward (negative amounts are ignored).
   * @return How many midnights were passed.
   */
  int advance(int minutes);

  /** @brief "HH:MM", e.g. "17:30". */
  std::string format() const;

 private:
  int minutes_;
};

#endif  // GAMECLOCK_HPP_INCLUDED
