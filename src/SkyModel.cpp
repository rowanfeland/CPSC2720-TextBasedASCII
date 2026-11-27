#include "SkyModel.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

#include "Rgb.hpp"
#include "SkyState.hpp"

namespace {

const double kPi = 3.14159265358979323846;
const int kMinutesPerDay = 24 * 60;
const int kSunrise = 6 * 60;
const int kSunset = 19 * 60;
const double kStarsFullBelow = 0.25;  // light level where stars are at full
const double kStarsStartBelow = 0.6;  // light level where stars first appear

const char kDefaultSky[] =
    "00:00 6,8,24 22,24,48 0.22\n"
    "05:00 6,10,30 30,30,60 0.25\n"
    "06:00 60,70,130 230,150,120 0.55\n"
    "08:00 70,120,200 200,200,210 0.95\n"
    "12:00 60,120,210 190,210,230 1.0\n"
    "16:00 50,100,190 220,200,170 1.0\n"
    "17:30 38,62,128 244,176,112 0.95\n"
    "19:00 30,30,80 180,90,80 0.6\n"
    "20:00 14,16,48 50,40,70 0.32\n";

bool toRgb(const std::string& s, Rgb& out) {
  int v[3];
  char comma1 = 0;
  char comma2 = 0;
  std::istringstream in(s);
  in >> v[0] >> comma1 >> v[1] >> comma2 >> v[2];
  if (in.fail() || comma1 != ',' || comma2 != ',' || !in.eof()) {
    return false;
  }
  out = Rgb(v[0], v[1], v[2]);
  return true;
}

bool toMinute(const std::string& s, int& out) {
  if (s.size() != 5 || s[2] != ':') {
    return false;
  }
  const int digits[] = {0, 1, 3, 4};
  if (!std::all_of(std::begin(digits), std::end(digits),
                   [&s](int i) { return s[i] >= '0' && s[i] <= '9'; })) {
    return false;
  }
  const int h = (s[0] - '0') * 10 + (s[1] - '0');
  const int m = (s[3] - '0') * 10 + (s[4] - '0');
  if (h > 23 || m > 59) {
    return false;
  }
  out = h * 60 + m;
  return true;
}

bool toLight(const std::string& s, double& out) {
  std::istringstream in(s);
  in >> out;
  return !in.fail() && in.eof();
}

}  // namespace

SkyModel::SkyModel() {
  std::ostringstream ignored;
  parse(kDefaultSky, ignored);
}

int SkyModel::parse(const std::string& text, std::ostream& warn) {
  std::vector<Key> keys;
  std::istringstream in(text);
  std::string line;
  int lineNo = 0;
  while (std::getline(in, line)) {
    ++lineNo;
    if (!line.empty() && line[line.size() - 1] == '\r') {
      line.erase(line.size() - 1);
    }
    std::istringstream fields(line);
    std::string time;
    std::string top;
    std::string horizon;
    std::string light;
    if (!(fields >> time) || time[0] == '#') {
      continue;
    }
    Key k;
    if (!(fields >> top >> horizon >> light) || !toMinute(time, k.minute) ||
        !toRgb(top, k.top) || !toRgb(horizon, k.horizon) ||
        !toLight(light, k.light)) {
      warn << "sky.txt line " << lineNo
           << " skipped: expected HH:MM r,g,b r,g,b light\n";
      continue;
    }
    k.light = std::min(1.0, std::max(0.0, k.light));
    keys.push_back(k);
  }
  if (!keys.empty()) {
    std::sort(keys.begin(), keys.end(),
              [](const Key& a, const Key& b) { return a.minute < b.minute; });
    keys_ = keys;
  }
  return static_cast<int>(keys.size());
}

bool SkyModel::load(const std::string& path, std::ostream& warn) {
  std::ifstream file(path.c_str());
  if (!file) {
    return false;
  }
  std::ostringstream buffer;
  buffer << file.rdbuf();
  parse(buffer.str(), warn);
  return true;
}

std::size_t SkyModel::size() const { return keys_.size(); }

SkyState SkyModel::sample(int minute) const {
  minute = ((minute % kMinutesPerDay) + kMinutesPerDay) % kMinutesPerDay;
  // Last keyframe at or before this minute, wrapping to yesterday's last one.
  std::size_t a = keys_.size() - 1;
  for (std::size_t i = 0; i < keys_.size(); ++i) {
    if (keys_[i].minute <= minute) {
      a = i;
    }
  }
  const std::size_t b = (a + 1) % keys_.size();
  int span = (keys_[b].minute - keys_[a].minute + kMinutesPerDay) % kMinutesPerDay;
  if (span == 0) {
    span = kMinutesPerDay;
  }
  const double t = static_cast<double>(
                       (minute - keys_[a].minute + kMinutesPerDay) % kMinutesPerDay) /
                   span;

  SkyState s;
  s.top = mix(keys_[a].top, keys_[b].top, t);
  s.horizon = mix(keys_[a].horizon, keys_[b].horizon, t);
  s.light = keys_[a].light + (keys_[b].light - keys_[a].light) * t;
  s.sunVisible = true;
  if (minute >= kSunrise && minute < kSunset) {
    const double p = static_cast<double>(minute - kSunrise) / (kSunset - kSunrise);
    s.moon = false;
    s.sunAzimuth = p * kPi;
    s.sunElevation = std::sin(p * kPi) * 0.95;
  } else {
    const int night = kMinutesPerDay - (kSunset - kSunrise);
    const double q = static_cast<double>(
                         (minute - kSunset + kMinutesPerDay) % kMinutesPerDay) /
                     night;
    s.moon = true;
    s.sunAzimuth = q * kPi;
    s.sunElevation = std::sin(q * kPi) * 0.8;
  }
  s.stars = std::min(1.0, std::max(0.0, (kStarsStartBelow - s.light) /
                                            (kStarsStartBelow - kStarsFullBelow)));
  return s;
}
