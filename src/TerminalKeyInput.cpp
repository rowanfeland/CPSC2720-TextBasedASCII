#include "TerminalKeyInput.hpp"

#include <poll.h>
#include <unistd.h>

#include <vector>

#include "Key.hpp"
#include "RawTerminal.hpp"

namespace {

/** How long to wait for the rest of an escape sequence, in milliseconds. */
const int kEscapeWaitMs = 30;

bool byteReady(int waitMs) {
  pollfd fd;
  fd.fd = STDIN_FILENO;
  fd.events = POLLIN;
  fd.revents = 0;
  return poll(&fd, 1, waitMs) > 0;
}

}  // namespace

Key TerminalKeyInput::readKey() {
  while (queued_.empty()) {
    if (RawTerminal::takeResize()) {
      return Key(KeyType::None);  // redraw at the new size
    }
    if (!decoder_.pending() && !byteReady(-1)) {
      continue;  // woken by a signal (e.g. resize)
    }
    if (decoder_.pending() && !byteReady(kEscapeWaitMs)) {
      std::vector<Key> flushed = decoder_.flush();
      queued_.insert(queued_.end(), flushed.begin(), flushed.end());
      continue;
    }
    char byte = 0;
    if (read(STDIN_FILENO, &byte, 1) != 1) {
      return Key(KeyType::Eof);
    }
    std::vector<Key> keys = decoder_.feed(byte);
    queued_.insert(queued_.end(), keys.begin(), keys.end());
  }
  const Key key = queued_.front();
  queued_.pop_front();
  return key;
}
