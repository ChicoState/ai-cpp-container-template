#ifndef RPS_GAME_HPP
#define RPS_GAME_HPP

#include <functional>
#include <istream>
#include <optional>
#include <ostream>
#include <string>
#include <vector>

enum class Move {
  Rock,
  Paper,
  Scissors,
};

enum class RoundResult {
  PlayerWin,
  ComputerWin,
  Tie,
};

struct Score {
  int playerWins = 0;
  int computerWins = 0;
};

using ComputerMoveProvider = std::function<Move()>;

std::optional<Move> parseMove(const std::string& input);
RoundResult evaluateRound(Move playerMove, Move computerMove);
void applyRoundResult(Score& score, RoundResult result);
std::vector<std::string> renderRound(Move playerMove, Move computerMove);
void runGame(std::istream& input, std::ostream& output,
             const ComputerMoveProvider& nextComputerMove);

#endif
