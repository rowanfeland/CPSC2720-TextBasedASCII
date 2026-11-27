/**
 * @file IKeyInput.hpp
 * @brief Source of key presses, so the exterior game can be tested without a
 * real keyboard.
 */
#ifndef IKEYINPUT_HPP_INCLUDED
#define IKEYINPUT_HPP_INCLUDED

#include "Key.hpp"

/** @brief Delivers one key at a time. */
class IKeyInput {
 public:
  virtual ~IKeyInput() {}

  /** @brief Waits for and returns the next key (Eof when input ends). */
  virtual Key readKey() = 0;
};

#endif  // IKEYINPUT_HPP_INCLUDED
