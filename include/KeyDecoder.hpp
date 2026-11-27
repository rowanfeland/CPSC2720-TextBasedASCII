/**
 * @file KeyDecoder.hpp
 * @brief Turns raw terminal bytes into keys, including arrow escape codes.
 */
#ifndef KEYDECODER_HPP_INCLUDED
#define KEYDECODER_HPP_INCLUDED

#include <vector>

#include "Key.hpp"

/**
 * @brief Byte-at-a-time decoder. Escape sequences may arrive split across
 * reads; the decoder keeps its place between calls.
 */
class KeyDecoder {
 public:
  /** @brief Starts with nothing pending. */
  KeyDecoder();

  /** @brief Feeds one byte; returns the keys it completes (often none or one). */
  std::vector<Key> feed(char byte);

  /** @brief True while an escape sequence is part-way through. */
  bool pending() const;

  /** @brief Gives up on a part-way sequence (e.g. a lone Esc press). */
  std::vector<Key> flush();

 private:
  enum class State { Normal, Escape, Csi };

  std::vector<Key> normal(char byte);

  State state_;
};

#endif  // KEYDECODER_HPP_INCLUDED
