/**
 * @file RawTerminal.hpp
 * @brief Puts the terminal into raw key-by-key mode for the 3D view, and
 * always puts it back.
 */
#ifndef RAWTERMINAL_HPP_INCLUDED
#define RAWTERMINAL_HPP_INCLUDED

/**
 * @brief While an instance exists: keys arrive one at a time without echo,
 * the alternate screen is shown and the cursor is hidden. Keys never raise
 * signals (Ctrl-C arrives as a key). The terminal is restored by the
 * destructor, at normal program exit, and on termination, hang-up, abort
 * and crash signals. Only one instance may exist at a time.
 */
class RawTerminal {
 public:
  /** @brief Enters raw mode. Does nothing if stdin is not a terminal. */
  RawTerminal();

  /** @brief Restores the terminal. */
  ~RawTerminal();

  RawTerminal(const RawTerminal&) = delete;
  RawTerminal& operator=(const RawTerminal&) = delete;

  /** @brief Current terminal size in character cells. false if unknown. */
  static bool size(int& cols, int& rows);

  /** @brief True when both stdin and stdout are a live terminal. */
  static bool isInteractive();

  /** @brief Restores the terminal now (safe to call more than once). */
  static void restore();

  /** @brief True once after the window has been resized. */
  static bool takeResize();
};

#endif  // RAWTERMINAL_HPP_INCLUDED
