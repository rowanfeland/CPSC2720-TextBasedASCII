#include "ExteriorGame.hpp"

#include <cctype>
#include <cmath>
#include <cstdio>
#include <sstream>
#include <string>
#include <vector>

#include "Camera.hpp"
#include "Command.hpp"
#include "Horse.hpp"
#include "CommandParser.hpp"
#include "Raycaster.hpp"
#include "Room.hpp"
#include "TerminalPainter.hpp"
#include "TextUtil.hpp"

namespace {

const double kPi = 3.14159265358979323846;
const char kTooSmall[] = "Make the terminal bigger (80 x 24 or more).";
const char kHelp[] =
    "Arrow keys walk and turn. Type a command, then press Enter.\n"
    "Commands: look, look <thing>, mount, dismount, wait, take, drop, "
    "inventory, help, quit";

std::string upper(const std::string& s) {
  std::string out = s;
  for (std::string::size_type i = 0; i < out.size(); ++i) {
    out[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(out[i])));
  }
  return out;
}

std::string compass(double angle) {
  static const char* names[] = {"east",  "south-east", "south", "south-west",
                                "west",  "north-west", "north", "north-east"};
  const int index =
      static_cast<int>(std::floor(angle / (kPi / 4) + 0.5)) % 8;
  return names[index];
}

std::string moveTo(int row) {
  char buf[32];
  std::snprintf(buf, sizeof(buf), "\033[%d;1H\033[0m\033[2K", row);
  return buf;
}

std::vector<std::string> wrap(const std::string& text, std::string::size_type width) {
  std::vector<std::string> lines;
  std::istringstream in(text);
  std::string paragraph;
  while (std::getline(in, paragraph)) {
    while (paragraph.size() > width) {
      std::string::size_type cut = paragraph.rfind(' ', width);
      if (cut == std::string::npos || cut == 0) {
        cut = width;
      }
      lines.push_back(paragraph.substr(0, cut));
      paragraph = paragraph.substr(cut);
      if (!paragraph.empty() && paragraph[0] == ' ') {
        paragraph.erase(0, 1);
      }
    }
    lines.push_back(paragraph);
  }
  return lines;
}

}  // namespace

ExteriorGame::ExteriorGame(const WorldMap& map, World& world, Player& player,
                           GameState& state, CommandRegistry& registry,
                           IKeyInput& keys, IScreen& screen, bool plain)
    : map_(map),
      world_(world),
      player_(player),
      state_(state),
      registry_(registry),
      keys_(keys),
      screen_(screen),
      plain_(plain),
      camera_(map.start().x, map.start().y, kPi),
      horse_(map.start(), true),
      previous_(0, 0) {
  updateRoom();
}

void ExteriorGame::run() {
  draw();
  while (handleKey(keys_.readKey())) {
    draw();
  }
}

bool ExteriorGame::handleKey(const Key& key) {
  switch (key.type) {
    case KeyType::Up:
      walk(1.0);
      break;
    case KeyType::Down:
      walk(-1.0);
      break;
    case KeyType::Left:
      camera_.turn(-kTurnStep);
      break;
    case KeyType::Right:
      camera_.turn(kTurnStep);
      break;
    case KeyType::Char:
      if (input_.size() < kMaxInput) {
        input_ += key.ch;
      }
      break;
    case KeyType::Backspace:
      if (!input_.empty()) {
        input_.erase(input_.size() - 1);
      }
      break;
    case KeyType::Enter:
      message_ = runCommand(input_);
      input_.clear();
      return !state_.quitRequested();
    case KeyType::Eof:
      return false;
    default:
      break;
  }
  return true;
}

std::string ExteriorGame::runCommand(const std::string& line) {
  const std::string typed = normalize(line);
  if (typed.empty()) {
    return "Type a command, or help.";
  }
  const ParsedCommand cmd = CommandParser::parse(registry_.expand(typed));
  if (cmd.verb == "look" && cmd.noun.empty()) {
    return describeHere();
  }
  if (cmd.verb == "help") {
    return kHelp;
  }
  if (cmd.verb == "go") {
    return "Use the arrow keys to walk.";
  }
  const std::string first = CommandParser::parse(typed).verb;
  if (first == "wait") {
    passTime(kWaitMinutes);
    return "You wait. Time passes.";
  }
  if (first == "mount" || first == "ride" || first == "dismount") {
    const bool wasMounted = horse_.isMounted();
    const Point2 here(camera_.x(), camera_.y());
    const std::string result =
        first == "dismount" ? horse_.dismount(map_, here, camera_.angle())
                            : horse_.mount(here);
    if (horse_.isMounted() != wasMounted) {
      passTime(kActionMinutes);
    }
    return result;
  }
  if (cmd.verb == "quit") {
    state_.requestQuit();
    return "You turn your horse around.";
  }
  Command* command = registry_.find(cmd.verb);
  if (command == nullptr) {
    return "I don't understand \"" + typed + "\". Type help.";
  }
  const std::string result = command->execute(cmd, player_, state_);
  if (result.compare(0, 6, "Taken:") == 0 ||
      result.compare(0, 8, "Dropped:") == 0) {
    passTime(kActionMinutes);
  }
  return result;
}

void ExteriorGame::draw() {
  int cols = kMinCols;
  int rows = kMinRows;
  screen_.size(cols, rows);
  if (cols < kMinCols || rows < kMinRows) {
    previous_ = FrameBuffer(0, 0);
    screen_.write(std::string("\033[0m\033[2J\033[H") + kTooSmall);
    return;
  }
  FrameBuffer frame(cols, (rows - kTextRows) * 2);
  RenderInput in(map_, camera_);
  in.eyeHeight = horse_.eyeHeight();
  in.riding = horse_.isMounted();
  in.showHorse = !horse_.isMounted();
  in.horse = horse_.position();
  in.sky = map_.sky().sample(clock_.minutes());
  renderView(in, frame, depth_);
  std::string out;
  if (plain_) {
    out = "\033[H" + TerminalPainter::paintPlain(frame);
  } else {
    out = TerminalPainter::paintDiff(previous_, frame, 1);
  }
  out += textArea(cols, rows);
  screen_.write(out);
  previous_ = frame;
}

const Camera& ExteriorGame::camera() const { return camera_; }

const Horse& ExteriorGame::horse() const { return horse_; }

const GameClock& ExteriorGame::clock() const { return clock_; }

void ExteriorGame::walk(double direction) {
  if (camera_.step(map_, direction * kWalkStep * horse_.speedFactor())) {
    passTime(kStepMinutes);
  }
  horse_.follow(Point2(camera_.x(), camera_.y()));
  updateRoom();
}

void ExteriorGame::passTime(int minutes) {
  const int midnights = clock_.advance(minutes);
  state_.setDay(state_.getDay() + midnights);
}

const std::string& ExteriorGame::inputLine() const { return input_; }

const std::string& ExteriorGame::lastMessage() const { return message_; }

void ExteriorGame::updateRoom() {
  const Zone* zone = map_.zoneAt(camera_.x(), camera_.y());
  if (zone == nullptr) {
    return;
  }
  Room* room = world_.getRoom(zone->roomId);
  if (room != nullptr) {
    player_.moveTo(room);
  }
}

std::string ExteriorGame::describeHere() const {
  const Zone* zone = map_.zoneAt(camera_.x(), camera_.y());
  std::string text = zone == nullptr ? "Open desert." : zone->description;
  return text + "\nYou are facing " + compass(camera_.angle()) + ".";
}

std::string ExteriorGame::statusLine() const {
  const Zone* zone = map_.zoneAt(camera_.x(), camera_.y());
  std::string place = "OPEN DESERT";
  if (zone != nullptr) {
    const Room* room = world_.getRoom(zone->roomId);
    place = upper(room != nullptr ? room->getName() : zone->roomId);
  }
  return place + "   " + clock_.format() +
         (horse_.isMounted() ? "   riding" : "   on foot");
}

std::string ExteriorGame::textArea(int cols, int rows) const {
  const std::string::size_type width = static_cast<std::string::size_type>(cols - 1);
  std::vector<std::string> lines = wrap(message_, width);
  std::string out = moveTo(rows - 3) + statusLine().substr(0, width);
  for (int i = 0; i < 2; ++i) {
    out += moveTo(rows - 2 + i);
    if (static_cast<std::vector<std::string>::size_type>(i) < lines.size()) {
      out += lines[i].substr(0, width);
    }
  }
  out += moveTo(rows) + ("> " + input_ + "_").substr(0, width);
  return out;
}
