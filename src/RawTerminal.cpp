#include "RawTerminal.hpp"

#include <signal.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>

namespace {

termios gSaved;
volatile sig_atomic_t gActive = 0;
volatile sig_atomic_t gResized = 0;
bool gHooksInstalled = false;

const char kEnter[] = "\033[?1049h\033[?25l\033[2J";
const char kLeave[] = "\033[0m\033[?25h\033[?1049l";

void writeAll(const char* text) {
  const ssize_t ignored = write(STDOUT_FILENO, text, std::strlen(text));
  (void)ignored;
}

void onSignal(int sig) {
  RawTerminal::restore();
  signal(sig, SIG_DFL);
  raise(sig);
}

void onExit() { RawTerminal::restore(); }

void onResize(int) { gResized = 1; }

void install(int sig, void (*handler)(int)) {
  struct sigaction action;
  std::memset(&action, 0, sizeof(action));
  action.sa_handler = handler;
  sigemptyset(&action.sa_mask);
  sigaction(sig, &action, nullptr);  // no SA_RESTART: wakes up poll()
}

}  // namespace

RawTerminal::RawTerminal() {
  if (!isatty(STDIN_FILENO) || tcgetattr(STDIN_FILENO, &gSaved) != 0) {
    return;
  }
  termios raw = gSaved;
  raw.c_lflag &= ~static_cast<tcflag_t>(ICANON | ECHO | ISIG);
  raw.c_iflag &= ~static_cast<tcflag_t>(IXON | ICRNL);
  raw.c_cc[VMIN] = 1;
  raw.c_cc[VTIME] = 0;
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
  gActive = 1;
  if (!gHooksInstalled) {
    std::atexit(onExit);
    const int fatal[] = {SIGINT, SIGTERM, SIGHUP, SIGQUIT, SIGABRT, SIGSEGV};
    for (int sig : fatal) {
      install(sig, onSignal);
    }
    install(SIGWINCH, onResize);
    gHooksInstalled = true;
  }
  writeAll(kEnter);
}

RawTerminal::~RawTerminal() { restore(); }

void RawTerminal::restore() {
  if (!gActive) {
    return;
  }
  gActive = 0;
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &gSaved);
  writeAll(kLeave);
}

bool RawTerminal::takeResize() {
  if (!gResized) {
    return false;
  }
  gResized = 0;
  return true;
}

bool RawTerminal::size(int& cols, int& rows) {
  winsize ws;
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) != 0 || ws.ws_col == 0) {
    return false;
  }
  cols = ws.ws_col;
  rows = ws.ws_row;
  return true;
}

bool RawTerminal::isInteractive() {
  return isatty(STDIN_FILENO) && isatty(STDOUT_FILENO);
}
