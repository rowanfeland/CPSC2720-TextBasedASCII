/**
 * @file TerminalPainter.hpp
 * @brief Turns a FrameBuffer into terminal output: two pixels per cell using
 * the upper-half block, with 24-bit colour, or plain density characters.
 */
#ifndef TERMINALPAINTER_HPP_INCLUDED
#define TERMINALPAINTER_HPP_INCLUDED

#include <string>

#include "FrameBuffer.hpp"

/** @brief Builds escape-code strings for frames. Writes nothing itself. */
class TerminalPainter {
 public:
  /**
   * @brief Every cell of the frame, starting at terminal row topRow (1-based).
   * Each cell row starts with a cursor move, so it does not depend on wrapping.
   */
  static std::string paintFull(const FrameBuffer& frame, int topRow);

  /**
   * @brief Only the cells that differ from prev. If the sizes differ, the
   * whole frame is painted. Returns "" when nothing changed.
   */
  static std::string paintDiff(const FrameBuffer& prev, const FrameBuffer& next,
                               int topRow);

  /** @brief Density characters (" .:-=+*#%@"), one per cell, no escape codes. */
  static std::string paintPlain(const FrameBuffer& frame);
};

#endif  // TERMINALPAINTER_HPP_INCLUDED
