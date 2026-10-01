#include "game.h"

#include <iostream>
#include <string>

int main() {
  Game game;

  std::cout << "=== Maze Escape ===\n"
            << "Collect K, avoid H, and reach E as quickly as you can.\n"
            << "Controls: W/A/S/D to move, Q to quit.\n";

  while (game.status() == GameStatus::Active) {
    const Game::Clock::time_point now = Game::Clock::now();
    std::cout << '\n' << game.render(now) << game.lastMessage() << '\n'
              << "Move: " << std::flush;

    std::string input;
    if (!std::getline(std::cin, input)) {
      game.quit(Game::Clock::now());
      break;
    }

    game.applyInput(input.empty() ? '\0' : input.front(), Game::Clock::now());
  }

  const Game::Clock::time_point finished = Game::Clock::now();
  std::cout << '\n' << game.render(finished) << game.lastMessage() << '\n';
  if (game.status() == GameStatus::Won) {
    std::cout << "Final time: " << formatDuration(game.elapsed(finished)) << '\n';
  }

  return 0;
}
