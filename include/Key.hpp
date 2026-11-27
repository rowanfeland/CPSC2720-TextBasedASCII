/**
 * @file Key.hpp
 * @brief One key press from the player, already decoded.
 */
#ifndef KEY_HPP_INCLUDED
#define KEY_HPP_INCLUDED

/** @brief What kind of key was pressed. */
enum class KeyType {
  Up,         ///< Arrow up: step forward.
  Down,       ///< Arrow down: step back.
  Left,       ///< Arrow left: turn left.
  Right,      ///< Arrow right: turn right.
  Char,       ///< A printable character for the command line.
  Backspace,  ///< Delete the last typed character.
  Enter,      ///< Run the typed command.
  Eof,        ///< Input ended (Ctrl-D or closed stream).
  None,       ///< A key the game ignores.
};

/** @brief A decoded key; ch is set only for KeyType::Char. */
struct Key {
  KeyType type;  ///< Kind of key.
  char ch;       ///< The character, for Char keys.

  /** @brief Creates a key. */
  explicit Key(KeyType t = KeyType::None, char c = '\0') : type(t), ch(c) {}
};

#endif  // KEY_HPP_INCLUDED
