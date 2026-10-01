#include "../game.h"

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>

namespace {

using Clock = Game::Clock;

int failures = 0;

void expect(bool condition, const std::string& message) {
  if (!condition) {
    std::cerr << "FAIL: " << message << '\n';
    ++failures;
  }
}

Clock::time_point at(int milliseconds) {
  return Clock::time_point{} + std::chrono::milliseconds(milliseconds);
}

void apply(Game& game, const std::string& commands, int start = 0) {
  for (std::size_t index = 0; index < commands.size(); ++index) {
    game.applyInput(commands[index], at(start + static_cast<int>(index) + 1));
  }
}

void testInvalidInputDoesNotStartTheTimer() {
  Game game;

  game.applyInput('x', at(10));
  game.applyInput('w', at(20));

  expect(game.playerPosition() == Position{1, 1},
         "invalid and blocked input leave the player at the spawn");
  expect(game.elapsed(at(1000)) == std::chrono::milliseconds::zero(),
         "timer stays stopped until a legal move");
}

void testLegalMoveStartsTimerAndWallsBlockMovement() {
  Game game;

  game.applyInput('d', at(100));
  game.applyInput('w', at(200));

  expect(game.playerPosition() == Position{1, 2},
         "a legal move advances the player by one tile");
  expect(game.elapsed(at(600)) == std::chrono::milliseconds(500),
         "timer measures time from the first legal move");
}

void testKeyUnlocksTheExitAndAKeylessExitStaysLocked() {
  Game lockedGame;
  apply(lockedGame, "ssddddddsss");

  expect(lockedGame.status() == GameStatus::Active,
         "reaching the exit without the key does not end the game");
  expect(!lockedGame.hasKey(), "keyless route does not grant the key");

  Game winningGame;
  apply(winningGame, "ddddddsssss");

  expect(winningGame.hasKey(), "walking over the key collects it");
  expect(winningGame.status() == GameStatus::Won,
         "key route followed by exit wins the game");
}

void testHazardCollisionAndTerminalTimerFreeze() {
  Game game;
  apply(game, "ssssddd", 100);

  expect(game.status() == GameStatus::Lost,
         "entering the hazard ends the game as a loss");
  const auto finalTime = game.elapsed(at(107));
  expect(game.elapsed(at(10000)) == finalTime,
         "loss freezes the timer permanently");
}

void testQuitFreezesTimerAndFormattingIsStable() {
  Game game;
  game.applyInput('d', at(100));
  game.quit(at(275));

  expect(game.status() == GameStatus::Quit,
         "quit changes the game into a terminal state");
  expect(game.elapsed(at(1000)) == std::chrono::milliseconds(175),
         "quit freezes elapsed time");
  expect(formatDuration(std::chrono::milliseconds(62123)) == "01:02.12",
         "duration formatting uses mm:ss.cc");
}

}  // namespace

int main() {
  testInvalidInputDoesNotStartTheTimer();
  testLegalMoveStartsTimerAndWallsBlockMovement();
  testKeyUnlocksTheExitAndAKeylessExitStaysLocked();
  testHazardCollisionAndTerminalTimerFreeze();
  testQuitFreezesTimerAndFormattingIsStable();

  if (failures != 0) {
    std::cerr << failures << " test(s) failed\n";
    return EXIT_FAILURE;
  }

  std::cout << "All game tests passed\n";
  return EXIT_SUCCESS;
}
