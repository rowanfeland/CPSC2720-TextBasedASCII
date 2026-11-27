#include "GameClock.hpp"

#include <cstdio>
#include <string>

namespace {

const int kMinutesPerDay = 24 * 60;

}  // namespace

GameClock::GameClock(int minutes)
    : minutes_(((minutes % kMinutesPerDay) + kMinutesPerDay) % kMinutesPerDay) {}

int GameClock::minutes() const { return minutes_; }

int GameClock::advance(int minutes) {
  if (minutes <= 0) {
    return 0;
  }
  const int total = minutes_ + minutes;
  minutes_ = total % kMinutesPerDay;
  return total / kMinutesPerDay;
}

std::string GameClock::format() const {
  char buf[16];
  std::snprintf(buf, sizeof(buf), "%02d:%02d", minutes_ / 60, minutes_ % 60);
  return buf;
}
