#include "rps_game.hpp"

#include <iostream>
#include <string>

namespace {

int failures = 0;

void check(bool condition, const std::string& message) {
  if (!condition) {
    std::cerr << "FAILED: " << message << '\n';
    ++failures;
  }
}

void testParseMove() {
  check(parseMove("r") == Move::Rock, "accepts r");
  check(parseMove(" P ") == Move::Paper, "trims and accepts P");
  check(parseMove("\tS\n") == Move::Scissors, "trims and accepts S");
  check(!parseMove(""), "rejects empty input");
  check(!parseMove(" \t "), "rejects whitespace-only input");
  check(!parseMove("rock"), "rejects full words");
  check(!parseMove("x"), "rejects unknown input");
}

void testRoundResults() {
  check(evaluateRound(Move::Rock, Move::Scissors) == RoundResult::PlayerWin,
        "rock beats scissors");
  check(evaluateRound(Move::Paper, Move::Rock) == RoundResult::PlayerWin,
        "paper beats rock");
  check(evaluateRound(Move::Scissors, Move::Paper) == RoundResult::PlayerWin,
        "scissors beats paper");
  check(evaluateRound(Move::Rock, Move::Paper) == RoundResult::ComputerWin,
        "paper beats rock for computer");
  check(evaluateRound(Move::Paper, Move::Scissors) == RoundResult::ComputerWin,
        "scissors beats paper for computer");
  check(evaluateRound(Move::Scissors, Move::Rock) == RoundResult::ComputerWin,
        "rock beats scissors for computer");
  check(evaluateRound(Move::Rock, Move::Rock) == RoundResult::Tie,
        "rock ties rock");
  check(evaluateRound(Move::Paper, Move::Paper) == RoundResult::Tie,
        "paper ties paper");
  check(evaluateRound(Move::Scissors, Move::Scissors) == RoundResult::Tie,
        "scissors ties scissors");
}

void testScoreUpdates() {
  Score score;
  applyRoundResult(score, RoundResult::Tie);
  check(score.playerWins == 0 && score.computerWins == 0,
        "tie does not change score");
  applyRoundResult(score, RoundResult::PlayerWin);
  applyRoundResult(score, RoundResult::ComputerWin);
  check(score.playerWins == 1 && score.computerWins == 1,
        "wins increment only their side");
}

void testRendererLayout() {
  const Move moves[] = {Move::Rock, Move::Paper, Move::Scissors};
  for (Move playerMove : moves) {
    for (Move computerMove : moves) {
      const std::vector<std::string> lines = renderRound(playerMove, computerMove);
      check(lines.size() == 5, "renders a heading and four card rows");
      for (const std::string& line : lines) {
        check(line.size() == 29, "keeps both cards and separator at fixed width");
        if (line.size() == 29) {
          check(line[13] == ' ' && line[14] == '|' && line[15] == ' ',
                "uses a separator between fixed-width cards");
        }
      }
      check(lines.front().find("Player") < lines.front().find("Computer"),
            "places player heading before computer heading");
    }
  }
}

}  // namespace

int main() {
  testParseMove();
  testRoundResults();
  testScoreUpdates();
  testRendererLayout();
  return failures == 0 ? 0 : 1;
}
