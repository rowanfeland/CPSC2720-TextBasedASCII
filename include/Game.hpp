/**
 * @file Game.hpp
 * @brief The turn loop: draw the screen, read a command, run it, repeat.
 */
#ifndef GAME_HPP_INCLUDED
#define GAME_HPP_INCLUDED

#include <string>

#include "CommandRegistry.hpp"
#include "GameState.hpp"
#include "IDisplay.hpp"
#include "IInput.hpp"
#include "Player.hpp"
#include "SceneLibrary.hpp"
#include "SceneRenderer.hpp"
#include "World.hpp"

/**
 * @brief Runs the game through the display and input interfaces, so the
 * whole loop can be tested without a terminal.
 */
class Game {
 public:
  /**
   * @param world All rooms; must outlive the game.
   * @param player The player; must outlive the game.
   * @param state Game state; must outlive the game.
   * @param registry Commands and shortcuts; must outlive the game.
   * @param scenes Scene pictures; must outlive the game.
   * @param display Where output goes.
   * @param input Where commands come from.
   * @param mode Grey pixels or plain characters.
   */
  Game(World& world, Player& player, GameState& state, CommandRegistry& registry,
       const SceneLibrary& scenes, IDisplay& display, IInput& input, ColourMode mode);

  /** @brief Plays until the player quits or input ends. */
  void run();

  /**
   * @brief Runs one typed line.
   * @return false once the game should end.
   */
  bool processTurn(const std::string& line);

  /** @brief The message from the last command ("" if none). */
  const std::string& lastMessage() const;

  /** @brief Clears the screen and draws the current room, text and prompt. */
  void showScreen();

 private:
  World& world_;
  Player& player_;
  GameState& state_;
  CommandRegistry& registry_;
  const SceneLibrary& scenes_;
  IDisplay& display_;
  IInput& input_;
  ColourMode mode_;
  std::string lastMessage_;
};

#endif  // GAME_HPP_INCLUDED
