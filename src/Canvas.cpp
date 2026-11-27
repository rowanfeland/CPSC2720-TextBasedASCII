#include "Canvas.hpp"

#include <string>

Canvas::Canvas(int width, int height, char fill)
    : width_(width < 0 ? 0 : width),
      height_(height < 0 ? 0 : height),
      rows_(height_, std::string(width_, fill)) {}

int Canvas::width() const { return width_; }

int Canvas::height() const { return height_; }

bool Canvas::inBounds(int x, int y) const {
  return x >= 0 && x < width_ && y >= 0 && y < height_;
}

void Canvas::put(int x, int y, char c) {
  if (inBounds(x, y)) {
    rows_[y][x] = c;
  }
}

char Canvas::get(int x, int y) const {
  return inBounds(x, y) ? rows_[y][x] : ' ';
}

void Canvas::drawText(int x, int y, const std::string& text) {
  for (std::string::size_type i = 0; i < text.size(); ++i) {
    put(x + static_cast<int>(i), y, text[i]);
  }
}

void Canvas::drawBox(const Rect& r) {
  if (r.width <= 0 || r.height <= 0) {
    return;
  }
  const int right = r.x + r.width - 1;
  const int bottom = r.y + r.height - 1;
  for (int x = r.x; x <= right; ++x) {
    put(x, r.y, '-');
    put(x, bottom, '-');
  }
  for (int y = r.y; y <= bottom; ++y) {
    put(r.x, y, '|');
    put(right, y, '|');
  }
  put(r.x, r.y, '+');
  put(right, r.y, '+');
  put(r.x, bottom, '+');
  put(right, bottom, '+');
}

void Canvas::blit(int x, int y, const std::string& block) {
  int row = y;
  int col = x;
  for (std::string::size_type i = 0; i < block.size(); ++i) {
    const char c = block[i];
    if (c == '\n') {
      ++row;
      col = x;
    } else {
      if (c != ' ' && c != '\r') {
        put(col, row, c);
      }
      ++col;
    }
  }
}

std::string Canvas::toString() const {
  std::string out;
  for (std::vector<std::string>::size_type i = 0; i < rows_.size(); ++i) {
    out += rows_[i];
    out += '\n';
  }
  return out;
}
