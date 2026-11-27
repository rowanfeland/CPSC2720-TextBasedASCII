/**
 * @file ExteriorGame.hpp
 * @brief The live first-person outdoor game: arrow keys move, typed commands
 * interact.
 */
#ifndef EXTERIORGAME_HPP_INCLUDED
#define EXTERIORGAME_HPP_INCLUDED

#include <string>
#include <vector>

#include "Camera.hpp"
#include "CommandRegistry.hpp"
#include "FrameBuffer.hpp"
#include "GameClock.hpp"
#include "GameState.hpp"
#include "Horse.hpp"
#include "IKeyInput.hpp"
#include "IScreen.hpp"
#include "Key.hpp"
#include "Player.hpp"
#include "SceneLibrary.hpp"
#include "World.hpp"
#include "WorldMap.hpp"

/** @brief Rows under the view: status, two message lines, the input line. */
const int kTextRows = 4;
/** @brief Smallest terminal the 3D view needs. */
const int kMinCols = 80;
/** @brief Smallest terminal the 3D view needs. */
const int kMinRows = 24;
/** @brief How close you must stand to a door to enter. */
const double kEnterRange = 1.5;
/** @brief Rows under the picture when inside a location. */
const int kInsideTextRows = 8;
/** @brief Longest command the input line accepts. */
const std::string::size_type kMaxInput = 60;

/**
 * @brief Runs the outdoor 3D view. Shares the player, rooms, game state and
 * commands with the location screens, so items and state carry over.
 */
class ExteriorGame {
 public:
  /**
   * @param map The outdoor world (collision, zones, doors).
   * @param world All rooms; zones name the outdoor ones.
   * @param player Moved between rooms as the camera crosses zones.
   * @param state Shared game state.
   * @param registry Commands for take, drop, inventory and examining items.
   * @param keys Where key presses come from.
   * @param screen Where frames are written.
   * @param plain True to draw density characters instead of colour.
   */
  ExteriorGame(const WorldMap& map, World& world, Player& player,
               GameState& state, CommandRegistry& registry, IKeyInput& keys,
               IScreen& screen, bool plain);

  /** @brief Draws, then handles keys until the player quits or input ends. */
  void run();

  /** @brief Handles one key. Returns false when the game should end. */
  bool handleKey(const Key& key);

  /** @brief Runs one typed command and returns the message to show. */
  std::string runCommand(const std::string& line);

  /** @brief Draws the view and the text rows to the screen. */
  void draw();

  /** @brief The player's eye. */
  const Camera& camera() const;

  /** @brief The player's horse. */
  const Horse& horse() const;

  /** @brief The time of day. */
  const GameClock& clock() const;

  /** @brief True while you are inside a location screen. */
  bool isInside() const;

  /** @brief Pictures for location screens (nullptr = placeholders). */
  void setScenes(const SceneLibrary* scenes);

  /** @brief What has been typed so far. */
  const std::string& inputLine() const;

  /** @brief The message from the last command. */
  const std::string& lastMessage() const;

 private:
  void walk(double direction);
  void passTime(int minutes);
  std::string enterDoor(const std::string& place);
  std::string leaveBuilding();
  std::string runInside(const std::string& typed);
  bool isOutdoorRoom(const Room* room) const;
  void drawInside(int cols, int rows);
  void updateRoom();
  std::string describeHere() const;
  std::string statusLine() const;
  std::string textArea(int cols, int rows) const;

  const WorldMap& map_;
  World& world_;
  Player& player_;
  GameState& state_;
  CommandRegistry& registry_;
  IKeyInput& keys_;
  IScreen& screen_;
  bool plain_;
  Camera camera_;
  Horse horse_;
  GameClock clock_;
  bool inside_;
  bool tooSmall_;
  const BuildingDef* entry_;
  const SceneLibrary* scenes_;
  std::string pictureKey_;
  std::string input_;
  std::string message_;
  FrameBuffer previous_;
  std::vector<double> depth_;
};

#endif  // EXTERIORGAME_HPP_INCLUDED
