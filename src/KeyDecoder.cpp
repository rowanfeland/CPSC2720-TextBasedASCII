#include "KeyDecoder.hpp"

#include <vector>

#include "Key.hpp"

namespace {

const char kEscape = '\033';
const char kDelete = 127;
const char kCtrlH = 8;
const char kCtrlD = 4;

std::vector<Key> one(KeyType type, char ch = '\0') {
  return std::vector<Key>(1, Key(type, ch));
}

}  // namespace

KeyDecoder::KeyDecoder() : state_(State::Normal) {}

bool KeyDecoder::pending() const { return state_ != State::Normal; }

std::vector<Key> KeyDecoder::flush() {
  if (state_ == State::Normal) {
    return std::vector<Key>();
  }
  state_ = State::Normal;
  return one(KeyType::None);
}

std::vector<Key> KeyDecoder::normal(char byte) {
  if (byte == kEscape) {
    state_ = State::Escape;
    return std::vector<Key>();
  }
  if (byte == '\r' || byte == '\n') {
    return one(KeyType::Enter);
  }
  if (byte == kDelete || byte == kCtrlH) {
    return one(KeyType::Backspace);
  }
  if (byte == kCtrlD) {
    return one(KeyType::Eof);
  }
  if (byte >= ' ' && byte <= '~') {
    return one(KeyType::Char, byte);
  }
  return std::vector<Key>();
}

std::vector<Key> KeyDecoder::feed(char byte) {
  switch (state_) {
    case State::Escape:
      if (byte == '[') {
        state_ = State::Csi;
        return std::vector<Key>();
      } else {
        state_ = State::Normal;
        std::vector<Key> out = one(KeyType::None);
        std::vector<Key> rest = normal(byte);
        out.insert(out.end(), rest.begin(), rest.end());
        return out;
      }
    case State::Csi:
      if ((byte >= '0' && byte <= '9') || byte == ';') {
        return std::vector<Key>();
      }
      state_ = State::Normal;
      switch (byte) {
        case 'A':
          return one(KeyType::Up);
        case 'B':
          return one(KeyType::Down);
        case 'C':
          return one(KeyType::Right);
        case 'D':
          return one(KeyType::Left);
        default:
          return one(KeyType::None);
      }
    default:
      return normal(byte);
  }
}
