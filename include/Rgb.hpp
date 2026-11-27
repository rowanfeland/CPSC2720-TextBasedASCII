/**
 * @file Rgb.hpp
 * @brief A 24-bit colour and small helpers for blending it.
 */
#ifndef RGB_HPP_INCLUDED
#define RGB_HPP_INCLUDED

/** @brief One colour with 0..255 red, green and blue channels. */
struct Rgb {
  unsigned char r;  ///< Red.
  unsigned char g;  ///< Green.
  unsigned char b;  ///< Blue.

  /** @brief Creates a colour; values are clamped to 0..255. */
  explicit Rgb(int red = 0, int green = 0, int blue = 0)
      : r(clamp(red)), g(clamp(green)), b(clamp(blue)) {}

  /** @brief True when all three channels match. */
  bool operator==(const Rgb& o) const {
    return r == o.r && g == o.g && b == o.b;
  }

  /** @brief True when any channel differs. */
  bool operator!=(const Rgb& o) const { return !(*this == o); }

  /** @brief Clamps a channel value to 0..255. */
  static unsigned char clamp(int v) {
    return static_cast<unsigned char>(v < 0 ? 0 : (v > 255 ? 255 : v));
  }
};

/** @brief Blends from a to b; t is clamped to 0..1 (0 gives a, 1 gives b). */
inline Rgb mix(const Rgb& a, const Rgb& b, double t) {
  if (t < 0) {
    t = 0;
  }
  if (t > 1) {
    t = 1;
  }
  return Rgb(static_cast<int>(a.r + (b.r - a.r) * t + 0.5),
             static_cast<int>(a.g + (b.g - a.g) * t + 0.5),
             static_cast<int>(a.b + (b.b - a.b) * t + 0.5));
}

/** @brief Multiplies every channel by k (negative k gives black). */
inline Rgb scale(const Rgb& c, double k) {
  if (k < 0) {
    k = 0;
  }
  return Rgb(static_cast<int>(c.r * k + 0.5), static_cast<int>(c.g * k + 0.5),
             static_cast<int>(c.b * k + 0.5));
}

#endif  // RGB_HPP_INCLUDED
