#include "ExteriorGame.hpp"

#include <algorithm>
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
#include "GreyImage.hpp"
#include "Raycaster.hpp"
#include "Room.hpp"
#include "SceneLibrary.hpp"
#include "SceneRenderer.hpp"
#include "TerminalPainter.hpp"
#include "TextUtil.hpp"

namespace {

const double kPi = 3.14159265358979323846;
const char kTooSmall[] = "Make the terminal bigger (80 x 24 or more).";
const char kIndoors[] = "You're indoors. Type leave to go back outside.";
const char kInsideHelp[] =
    "Type commands, then press Enter.\n"
    "Commands: look, look <thing>, go <exit>, take, drop, inventory, wait, "
    "leave, help, quit";
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

double facingAway(char facing) {
  switch (facing) {
    case 'N':
      return kPi * 1.5;
    case 'E':
      return 0.0;
    case 'W':
      return kPi;
    default:
      return kPi / 2;
  }
}

bool namesDoor(const BuildingDef& b, const std::string& place) {
  const std::string label = normalize(b.label);
  return place == b.id || place == b.roomId || label == place ||
         b.id.compare(0, place.size(), place) == 0 ||
         label.find(place) != std::string::npos;
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
      inside_(false),
      entry_(nullptr),
      scenes_(nullptr),
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
  const bool arrow = key.type == KeyType::Up ||
                     key.type == KeyType::Down ||
                     key.type == KeyType::Left ||
                     key.type == KeyType::Right;
  if (inside_ && arrow) {
    message_ = kIndoors;
    return true;
  }
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
  if (inside_) {
    return runInside(typed);
  }
  const ParsedCommand cmd = CommandParser::parse(registry_.expand(typed));
  const ParsedCommand raw = CommandParser::parse(typed);
  if (raw.verb == "enter") {
    return enterDoor(raw.noun);
  }
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
    pictureKey_.clear();
    screen_.write(std::string("\033[0m\033[2J\033[H") + kTooSmall);
    return;
  }
  if (inside_) {
    drawInside(cols, rows);
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

bool ExteriorGame::isInside() const { return inside_; }

void ExteriorGame::setScenes(const SceneLibrary* scenes) { scenes_ = scenes; }

std::string ExteriorGame::enterDoor(const std::string& place) {
  if (horse_.isMounted()) {
    return "Dismount first.";
  }
  const BuildingDef* door =
      map_.doorNear(camera_.x(), camera_.y(), kEnterRange);
  if (door == nullptr) {
    return "There's no door here.";
  }
  if (!place.empty() && !namesDoor(*door, place)) {
    return "You're not at the " + place + ".";
  }
  Room* room = world_.getRoom(door->roomId);
  if (room == nullptr) {
    return "That door won't open.";
  }
  std::string why;
  if (!room->canEnter(why)) {
    return why;
  }
  player_.moveTo(room);
  inside_ = true;
  entry_ = door;
  pictureKey_.clear();
  passTime(kActionMinutes);
  return "";
}

std::string ExteriorGame::leaveBuilding() {
  inside_ = false;
  if (entry_ != nullptr) {
    camera_.setPosition(entry_->doorX + 0.5, entry_->doorY + 0.5);
    camera_.setAngle(facingAway(entry_->facing));
  }
  previous_ = FrameBuffer(0, 0);
  updateRoom();
  passTime(kActionMinutes);
  return "You step back outside.";
}

bool ExteriorGame::isOutdoorRoom(const Room* room) const {
  return room != nullptr &&
         std::any_of(map_.zones().begin(), map_.zones().end(),
                     [room](const Zone& z) { return z.roomId == room->getId(); });
}

std::string ExteriorGame::runInside(const std::string& typed) {
  const std::string first = CommandParser::parse(typed).verb;
  if (first == "leave" || first == "exit" || first == "out") {
    return leaveBuilding();
  }
  if (first == "wait") {
    passTime(kWaitMinutes);
    return "You wait. Time passes.";
  }
  if (first == "mount" || first == "ride" || first == "dismount") {
    return "Your horse is waiting outside.";
  }
  const ParsedCommand cmd = CommandParser::parse(registry_.expand(typed));
  if (cmd.verb == "help") {
    return kInsideHelp;
  }
  if (cmd.verb == "quit") {
    state_.requestQuit();
    return "You turn your horse around.";
  }
  Command* command = registry_.find(cmd.verb);
  if (command == nullptr) {
    return "I don't understand \"" + typed + "\". Type help.";
  }
  const Room* before = player_.getCurrentRoom();
  const std::string result = command->execute(cmd, player_, state_);
  if (player_.getCurrentRoom() != before) {
    if (isOutdoorRoom(player_.getCurrentRoom())) {
      return leaveBuilding();
    }
    passTime(kActionMinutes);
  } else if (result.compare(0, 6, "Taken:") == 0 ||
             result.compare(0, 8, "Dropped:") == 0) {
    passTime(kActionMinutes);
  }
  return result;
}

void ExteriorGame::drawInside(int cols, int rows) {
  const Room* room = player_.getCurrentRoom();
  const int pictureRows = rows - kInsideTextRows;
  std::string out;
  const std::string key =
      room->getId() + "/" +
      std::to_string(static_cast<int>(room->sceneKind())) + "/" +
      std::to_string(state_.getDay());
  if (key != pictureKey_) {
    const GreyImage source =
        scenes_ != nullptr
            ? scenes_->sceneFor(room->getId(), room->sceneKind())
            : makePlaceholder(room->sceneKind());
    GreyImage fitted(cols, pictureRows);
    for (int y = 0; y < pictureRows; ++y) {
      for (int x = 0; x < cols; ++x) {
        fitted.set(x, y, source.get(x * source.width() / cols,
                                    y * source.height() / pictureRows));
      }
    }
    out = "\033[0m\033[2J\033[H" +
          renderScene(fitted, state_.getDay(),
                      plain_ ? ColourMode::Plain : ColourMode::Grey);
    pictureKey_ = key;
  }
  const std::string::size_type width =
      static_cast<std::string::size_type>(cols - 1);
  std::vector<std::string> lines = wrap(room->describe(), width);
  if (!message_.empty()) {
    const std::vector<std::string> extra = wrap(message_, width);
    lines.insert(lines.end(), extra.begin(), extra.end());
  }
  out += moveTo(pictureRows + 1) +
         (upper(room->getName()) + "   " + clock_.format()).substr(0, width);
  const int textRows = kInsideTextRows - 2;
  const int skip = std::max(0, static_cast<int>(lines.size()) - textRows);
  for (int i = 0; i < textRows; ++i) {
    out += moveTo(pictureRows + 2 + i);
    const int at = skip + i;
    if (at < static_cast<int>(lines.size())) {
      out += lines[at].substr(0, width);
    }
  }
  out += moveTo(rows) + ("> " + input_ + "_").substr(0, width);
  screen_.write(out);
}

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
