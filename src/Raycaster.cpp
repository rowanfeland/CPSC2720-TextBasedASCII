#include "Raycaster.hpp"

#include <algorithm>
#include <cmath>
#include <vector>

#include "Camera.hpp"
#include "FrameBuffer.hpp"
#include "Rgb.hpp"
#include "WallTextures.hpp"
#include "WorldMap.hpp"

namespace {

const double kPi = 3.14159265358979323846;
const double kNoWall = 1e9;
const double kFogDistance = 34.0;
const double kFogMax = 0.75;
const double kSideShade = 0.72;
const double kFov = 0.66;
const int kMaxRaySteps = 160;
const int kMaxHits = 8;
const Rgb kMountain(124, 86, 116);
const Rgb kStar(250, 250, 235);

unsigned hash2(int a, int b) {
  unsigned n = static_cast<unsigned>(a) * 374761393u +
               static_cast<unsigned>(b) * 668265263u;
  n = (n ^ (n >> 13)) * 1274126177u;
  return n ^ (n >> 16);
}

double fogAmount(double dist) {
  return std::pow(std::min(1.0, dist / kFogDistance), 1.1) * kFogMax;
}

double wrapAngle(double a) {
  while (a > kPi) {
    a -= 2 * kPi;
  }
  while (a < -kPi) {
    a += 2 * kPi;
  }
  return a;
}

/** World angle seen through screen column x. */
double columnAngle(const Camera& cam, int x, int width) {
  const double cx = 2.0 * x / width - 1.0;
  return cam.angle() + std::atan(cx * kFov);
}

Rgb groundColour(Ground g, double wx, double wy) {
  const double n = (hash2(static_cast<int>(std::floor(wx * 6)),
                          static_cast<int>(std::floor(wy * 6))) % 100) / 100.0;
  switch (g) {
    case Ground::Street:
      return scale(Rgb(178, 132, 86), 0.95 + 0.06 * n);
    case Ground::Boardwalk: {
      const Rgb c(128, 88, 54);
      return (wx * 3 - std::floor(wx * 3)) < 0.12 ? scale(c, 0.65) : c;
    }
    case Ground::Grass:
      return scale(Rgb(150, 138, 78), 0.85 + 0.25 * n);
    case Ground::Salt:
      return scale(Rgb(236, 232, 220), 0.96 + 0.04 * n);
    default:
      return scale(Rgb(214, 178, 126), 0.95 + 0.05 * n);
  }
}

void drawSky(const RenderInput& in, FrameBuffer& f, double horizon) {
  const int w = f.width();
  for (int y = 0; y < horizon && y < f.height(); ++y) {
    const Rgb row = mix(in.sky.top, in.sky.horizon, std::pow(y / horizon, 1.6));
    for (int x = 0; x < w; ++x) {
      Rgb c = row;
      if (in.sky.stars > 0) {
        const int sx = static_cast<int>(columnAngle(in.cam, x, w) * w * 0.8);
        if (hash2(sx, y) % 1000 < in.sky.stars * 8) {
          c = mix(c, kStar, 0.85);
        }
      }
      f.set(x, y, c);
    }
  }
}

void drawSunOrMoon(const RenderInput& in, FrameBuffer& f, double horizon) {
  if (!in.sky.sunVisible) {
    return;
  }
  const double diff = wrapAngle(in.sky.sunAzimuth - in.cam.angle());
  if (std::fabs(diff) >= kPi / 2) {
    return;
  }
  const int w = f.width();
  const double cx = w / 2.0 * (1 + std::tan(diff) / kFov);
  const double cy = horizon - in.sky.sunElevation * horizon * 0.9;
  const double radius = f.height() * 0.075;
  const Rgb disc = in.sky.moon ? kMoonColour : kSunColour;
  for (int y = 0; y < horizon; ++y) {
    for (int x = 0; x < w; ++x) {
      const double d = std::hypot(x - cx, y - cy);
      if (d < radius) {
        f.set(x, y, disc);
      } else if (d < radius * 1.7) {
        f.set(x, y, mix(f.get(x, y), disc, (radius * 1.7 - d) / (radius * 1.4)));
      }
    }
  }
}

void drawMountains(const RenderInput& in, FrameBuffer& f, double horizon) {
  const double k = f.height() / 80.0;
  const double shade = 0.5 + 0.5 * in.sky.light;
  for (int x = 0; x < f.width(); ++x) {
    const double a = columnAngle(in.cam, x, f.width());
    const double ridge = horizon - k * (5 + 3.5 * std::sin(a * 6 + 0.4) +
                                        2.2 * std::sin(a * 17 + 2) +
                                        1.2 * std::sin(a * 41));
    for (int y = std::max(0, static_cast<int>(ridge)); y < horizon; ++y) {
      const double t = 0.35 + 0.25 * (y - ridge) / (8 * k);
      f.set(x, y, mix(scale(kMountain, shade), in.sky.horizon, t));
    }
  }
}

void drawGround(const RenderInput& in, FrameBuffer& f, double horizon,
                double focal) {
  const int w = f.width();
  for (int y = static_cast<int>(horizon); y < f.height(); ++y) {
    const double dist = in.eyeHeight * focal / (y - horizon + 0.5);
    for (int x = 0; x < w; ++x) {
      const double cx = 2.0 * x / w - 1.0;
      const double wx = in.cam.x() + dist * (in.cam.dirX() + in.cam.planeX() * cx);
      const double wy = in.cam.y() + dist * (in.cam.dirY() + in.cam.planeY() * cx);
      const Rgb c = scale(groundColour(in.map->groundAt(wx, wy), wx, wy),
                          in.sky.light);
      f.set(x, y, mix(c, in.sky.horizon, fogAmount(dist)));
    }
  }
}

const BuildingDef* wallAt(const WorldMap& map, int x, int y) {
  const BuildingDef* b = map.buildingAt(x, y);
  if (b != nullptr) {
    return b;
  }
  const char c = map.tile(x, y);
  if (c == '#') {
    return &rockWall();
  }
  if (c == 'F') {
    return &fenceWall();
  }
  return nullptr;
}

bool isFacade(const BuildingDef& b, int side, double rdx, double rdy) {
  if (b.style == "fence") {
    return true;
  }
  switch (b.facing) {
    case 'S':
      return side == 1 && rdy < 0;
    case 'N':
      return side == 1 && rdy > 0;
    case 'E':
      return side == 0 && rdx < 0;
    default:
      return side == 0 && rdx > 0;
  }
}

void drawWallColumn(const RenderInput& in, FrameBuffer& f,
                    std::vector<double>& depth, int x, double horizon,
                    double focal) {
  const WorldMap& map = *in.map;
  const int w = f.width();
  const double cx = 2.0 * x / w - 1.0;
  const double rdx = in.cam.dirX() + in.cam.planeX() * cx;
  const double rdy = in.cam.dirY() + in.cam.planeY() * cx;
  const double px = in.cam.x();
  const double py = in.cam.y();
  int mx = static_cast<int>(std::floor(px));
  int my = static_cast<int>(std::floor(py));
  const double ddx = rdx == 0 ? 1e30 : std::fabs(1 / rdx);
  const double ddy = rdy == 0 ? 1e30 : std::fabs(1 / rdy);
  const int sx = rdx < 0 ? -1 : 1;
  const int sy = rdy < 0 ? -1 : 1;
  double sdx = rdx < 0 ? (px - mx) * ddx : (mx + 1 - px) * ddx;
  double sdy = rdy < 0 ? (py - my) * ddy : (my + 1 - py) * ddy;
  const bool night = in.sky.light < 0.45;
  double clipBottom = f.height();
  const BuildingDef* previous = nullptr;
  int hits = 0;

  for (int step = 0; step < kMaxRaySteps && hits < kMaxHits; ++step) {
    int side = 0;
    if (sdx < sdy) {
      sdx += ddx;
      mx += sx;
    } else {
      sdy += ddy;
      my += sy;
      side = 1;
    }
    if (mx < 0 || my < 0 || mx >= map.width() || my >= map.height()) {
      return;
    }
    const BuildingDef* hit = wallAt(map, mx, my);
    if (hit == nullptr || hit == previous) {
      previous = hit;
      continue;
    }
    previous = hit;
    ++hits;
    const double perp = std::max(0.05, side == 0 ? sdx - ddx : sdy - ddy);
    const double line = focal / perp;
    const double bottom = horizon + in.eyeHeight * line;
    const double top = bottom - hit->height * line;
    const double u = side == 0 ? py + perp * rdy : px + perp * rdx;
    const bool facade = isFacade(*hit, side, rdx, rdy);
    const double fog = fogAmount(perp);
    const int from = std::max(0, static_cast<int>(std::ceil(top)));
    const int to = std::min(static_cast<int>(clipBottom),
                            static_cast<int>(std::ceil(bottom)));
    for (int y = from; y < to; ++y) {
      const double v = (bottom - y - 0.5) / line;
      bool glows = false;
      Rgb c = wallTexel(*hit, u, v, facade, night, glows);
      if (!facade) {
        c = scale(c, kSideShade);
      }
      if (!glows) {
        c = scale(c, in.sky.light);
      }
      f.set(x, y, mix(c, in.sky.horizon, fog));
      depth[static_cast<std::vector<double>::size_type>(y) * w + x] = perp;
    }
    clipBottom = std::min(clipBottom, std::max(0.0, top));
    if (clipBottom <= 0) {
      return;
    }
  }
}

}  // namespace

SkyState::SkyState()
    : top(38, 62, 128),
      horizon(244, 176, 112),
      light(1.0),
      sunAzimuth(kPi),
      sunElevation(0.35),
      sunVisible(true),
      moon(false),
      stars(0.0) {}

RenderInput::RenderInput(const WorldMap& world, const Camera& camera)
    : map(&world), cam(camera), eyeHeight(0.5) {}

void renderView(const RenderInput& in, FrameBuffer& frame,
                std::vector<double>& depth) {
  const int w = frame.width();
  const int h = frame.height();
  depth.assign(static_cast<std::vector<double>::size_type>(w) * h, kNoWall);
  if (w == 0 || h == 0) {
    return;
  }
  const double horizon = h * kHorizonFraction;
  const double focal = w * 0.5;
  drawSky(in, frame, horizon);
  drawSunOrMoon(in, frame, horizon);
  drawMountains(in, frame, horizon);
  drawGround(in, frame, horizon, focal);
  for (int x = 0; x < w; ++x) {
    drawWallColumn(in, frame, depth, x, horizon, focal);
  }
}
