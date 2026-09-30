#include "rps_game.hpp"

#include <iostream>
#include <random>

int main() {
  std::mt19937 engine(std::random_device{}());
  std::uniform_int_distribution<int> distribution(0, 2);

  const ComputerMoveProvider nextComputerMove = [&engine, &distribution]() {
    switch (distribution(engine)) {
      case 0:
        return Move::Rock;
      case 1:
        return Move::Paper;
      default:
        return Move::Scissors;
    }
  };

  runGame(std::cin, std::cout, nextComputerMove);
  return 0;
}
