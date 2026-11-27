/**
 * @file TerminalKeyInput.hpp
 * @brief Reads keys from the real terminal (use together with RawTerminal).
 */
#ifndef TERMINALKEYINPUT_HPP_INCLUDED
#define TERMINALKEYINPUT_HPP_INCLUDED

#include <deque>

#include "IKeyInput.hpp"
#include "Key.hpp"
#include "KeyDecoder.hpp"

/** @brief Reads stdin byte by byte and decodes keys. */
class TerminalKeyInput : public IKeyInput {
 public:
  /** @brief Waits for the next key. A lone Esc becomes KeyType::None. */
  Key readKey() override;

 private:
  KeyDecoder decoder_;
  std::deque<Key> queued_;
};

#endif  // TERMINALKEYINPUT_HPP_INCLUDED
