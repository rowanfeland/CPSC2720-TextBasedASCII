/**
 * @file SkyModel.hpp
 * @brief Sky colours and light through the day, from data/world/sky.txt.
 */
#ifndef SKYMODEL_HPP_INCLUDED
#define SKYMODEL_HPP_INCLUDED

#include <ostream>
#include <string>
#include <vector>

#include "Rgb.hpp"
#include "SkyState.hpp"

/**
 * @brief Keyframes of sky colour and light at times of day, blended in
 * between. Also places the sun by day and the moon and stars by night.
 */
class SkyModel {
 public:
  /** @brief Starts with built-in keyframes (dawn, noon, sunset, night). */
  SkyModel();

  /**
   * @brief Reads keyframe lines "HH:MM top_rgb horizon_rgb light".
   * Replaces the current keyframes if at least one line is good.
   * @return Number of good lines. Bad lines are reported on warn.
   */
  int parse(const std::string& text, std::ostream& warn);

  /** @brief Reads a sky file. false if it can't be opened. */
  bool load(const std::string& path, std::ostream& warn);

  /** @brief Number of keyframes. */
  std::size_t size() const;

  /** @brief The sky at a minute of the day (0..1439). */
  SkyState sample(int minute) const;

 private:
  struct Key {
    int minute;
    Rgb top;
    Rgb horizon;
    double light;
  };

  std::vector<Key> keys_;
};

#endif  // SKYMODEL_HPP_INCLUDED
