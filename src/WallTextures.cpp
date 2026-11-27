#include "WallTextures.hpp"

#include <cmath>
#include <string>

#include "Rgb.hpp"
#include "WorldMap.hpp"

namespace {

const Rgb kGlass(34, 44, 66);
const Rgb kGlint(150, 170, 190);
const Rgb kLamp(255, 204, 120);
const Rgb kDoor(70, 42, 28);
const Rgb kDark(34, 22, 16);
const Rgb kSignBoard(226, 206, 158);
const Rgb kSignInk(52, 34, 24);

unsigned hash3(int a, int b, int c) {
  unsigned n = static_cast<unsigned>(a) * 374761393u +
               static_cast<unsigned>(b) * 668265263u +
               static_cast<unsigned>(c) * 2147483647u;
  n = (n ^ (n >> 13)) * 1274126177u;
  return n ^ (n >> 16);
}

double noise(int a, int b) { return (hash3(a, b, 7) % 100) / 100.0; }

double frac(double v) { return v - std::floor(v); }

BuildingDef makeDef(const std::string& style, double height, const Rgb& base,
                    const Rgb& trim) {
  BuildingDef d;
  d.letter = '#';
  d.id = style;
  d.style = style;
  d.height = height;
  d.base = base;
  d.trim = trim;
  d.doorX = -1000;
  d.doorY = -1000;
  d.facing = 'S';
  return d;
}

int doorCell(const BuildingDef& b) {
  return (b.facing == 'E' || b.facing == 'W') ? b.doorY : b.doorX;
}

Rgb rockTexel(const BuildingDef& b, double u, double v) {
  const double n = noise(static_cast<int>(std::floor(u * 3)),
                         static_cast<int>(std::floor(v * 3)));
  Rgb c = scale(b.base, 0.72 + 0.36 * n);
  if (frac(u * 3 + v * 1.7) < 0.08) {
    c = scale(c, 0.7);  // cracks
  }
  return c;
}

Rgb fenceTexel(const BuildingDef& b, double u, double v) {
  const bool picket = frac(u * 5) < 0.55;
  if (v > b.height - 0.12 && !picket) {
    return Rgb(40, 36, 30);
  }
  return picket ? b.base : Rgb(54, 46, 38);
}

Rgb plankBase(const BuildingDef& b, double u, double v) {
  const int cell = static_cast<int>(std::floor(u));
  if (b.style == "adobe" || b.style == "church") {
    return scale(b.base, 0.92 + 0.08 * noise(static_cast<int>(u * 9),
                                             static_cast<int>(v * 9)));
  }
  if (b.style == "well") {
    const int row = static_cast<int>(v * 6);
    const int brick = static_cast<int>(u * 3 + (row % 2) * 0.5);
    Rgb c = scale(b.base, 0.8 + 0.25 * noise(brick, row));
    if (frac(v * 6) < 0.15 || frac(u * 3 + (row % 2) * 0.5) < 0.08) {
      c = scale(c, 0.6);
    }
    return c;
  }
  const int row = static_cast<int>(v * 9);
  Rgb c = scale(b.base, 0.86 + 0.14 * noise(row, cell));
  if (frac(v * 9) < 0.13) {
    c = scale(c, 0.72);
  }
  if (b.style == "barn" && frac(u * 6) < 0.08) {
    c = scale(c, 0.7);
  }
  return c;
}

bool inSign(const BuildingDef& b, double v) {
  return b.style != "church" && b.style != "well" && b.style != "mine" &&
         v > b.height - 0.38 && v < b.height - 0.12;
}

Rgb doorTexel(const BuildingDef& b, double fu, double v) {
  if (b.style == "saloon" && !(v > 0.22 && v < 0.62)) {
    return kDark;  // swinging half-doors
  }
  if (b.style == "barn") {
    const double across = (fu - 0.22) / 0.56;
    const double up = v / 0.82;
    const bool brace = std::fabs(across - up) < 0.06 ||
                       std::fabs(across - (1 - up)) < 0.06;
    return brace ? b.trim : Rgb(120, 46, 36);
  }
  if (b.style == "mine") {
    return (fu < 0.3 || fu > 0.7 || v > 0.75) ? Rgb(92, 64, 38) : Rgb(8, 6, 6);
  }
  if (fu < 0.27 || fu > 0.73 || v > 0.77) {
    return b.trim;
  }
  return kDoor;
}

/** Window texel, or false when (u, v) is not on a window. */
bool windowTexel(const BuildingDef& b, double u, double v, bool night,
                 Rgb& out, bool& glows) {
  if (b.style == "mine" || b.style == "well") {
    return false;
  }
  const double fu = frac(u);
  const double bands[2][2] = {{0.38, 0.8}, {1.05, 1.45}};
  for (int i = 0; i < 2; ++i) {
    double lo = bands[i][0];
    double hi = bands[i][1];
    if (b.style == "church" && i == 1) {
      lo = 1.2;
      hi = 1.9;
    }
    if (hi > b.height - 0.4 || !(fu > 0.26 && fu < 0.74 && v > lo && v < hi)) {
      continue;
    }
    if (fu < 0.31 || fu > 0.69 || v < lo + 0.05 || v > hi - 0.05) {
      out = b.trim;
      return true;
    }
    if (night && hash3(static_cast<int>(std::floor(u)), i, b.letter) % 3 != 0) {
      out = kLamp;
      glows = true;
      return true;
    }
    const double glint = (fu - 0.31) + (v - lo) * 0.6;
    out = (glint > 0.18 && glint < 0.24) ? kGlint : kGlass;
    return true;
  }
  return false;
}

}  // namespace

const BuildingDef& rockWall() {
  static const BuildingDef def =
      makeDef("rock", 2.6, Rgb(124, 100, 82), Rgb(90, 72, 60));
  return def;
}

const BuildingDef& fenceWall() {
  static const BuildingDef def =
      makeDef("fence", 0.38, Rgb(232, 230, 220), Rgb(60, 55, 50));
  return def;
}

Rgb wallTexel(const BuildingDef& b, double u, double v, bool facade, bool night,
              bool& glows) {
  glows = false;
  if (b.style == "rock") {
    return rockTexel(b, u, v);
  }
  if (b.style == "fence") {
    return fenceTexel(b, u, v);
  }
  if (v > b.height - 0.07) {
    return scale(b.trim, 0.55);  // roofline cap
  }
  if (b.style == "mine") {
    if (facade && static_cast<int>(std::floor(u)) == doorCell(b) && v < 1.1) {
      return doorTexel(b, frac(u), v);
    }
    return rockTexel(b, u, v);
  }
  if (facade && inSign(b, v)) {
    const bool letter = hash3(static_cast<int>(u * 7), static_cast<int>(v * 22), 1) % 3 == 0;
    return letter ? kSignInk : kSignBoard;
  }
  const Rgb base = plankBase(b, u, v);
  if (!facade) {
    return base;
  }
  if (static_cast<int>(std::floor(u)) == doorCell(b) && frac(u) > 0.22 &&
      frac(u) < 0.78 && v < 0.82) {
    return doorTexel(b, frac(u), v);
  }
  Rgb window;
  if (windowTexel(b, u, v, night, window, glows)) {
    return window;
  }
  if (v < 0.1) {
    return scale(base, 0.6);  // shadow under the boardwalk roof
  }
  return base;
}
