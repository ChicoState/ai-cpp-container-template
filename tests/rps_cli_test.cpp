#include "rps_game.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace {

int failures = 0;

void check(bool condition, const std::string& message) {
  if (!condition) {
    std::cerr << "FAILED: " << message << '\n';
    ++failures;
  }
}

int countOccurrences(const std::string& text, const std::string& needle) {
  int count = 0;
  std::size_t position = 0;
  while ((position = text.find(needle, position)) != std::string::npos) {
    ++count;
    position += needle.size();
  }
  return count;
}

ComputerMoveProvider fixedMoves(std::vector<Move> moves) {
  return [moves = std::move(moves), index = std::size_t{0}]() mutable {
    return moves.at(index++);
  };
}

void testMatchReplayAndQuit() {
  std::istringstream input("invalid\n R \nP\nP\n Y \nS\nP\nP\nn\n");
  std::ostringstream output;
  runGame(input, output,
          fixedMoves({Move::Rock, Move::Rock, Move::Rock, Move::Paper,
                      Move::Rock, Move::Rock}));

  const std::string transcript = output.str();
  check(transcript.find("Invalid move. Enter r, p, or s.") != std::string::npos,
        "re-prompts after invalid move");
  check(transcript.find("Round tied. Score: Player 0 - Computer 0") !=
            std::string::npos,
        "tie leaves score unchanged");
  check(countOccurrences(transcript, "New match!") == 2,
        "starts a new match after normalized replay input");
  check(countOccurrences(transcript, "You win the match!") == 2,
        "completes both deterministic matches");
  check(transcript.find("Player") != std::string::npos &&
            transcript.find("Computer") != std::string::npos,
        "renders labeled player and computer cards");
  check(transcript.find("Thanks for playing!") != std::string::npos,
        "quits after non-y replay input");
}

void testEndOfFileExitsCleanly() {
  std::istringstream input("");
  std::ostringstream output;
  runGame(input, output, fixedMoves({Move::Rock}));

  check(output.str().find("Thanks for playing!") != std::string::npos,
        "exits cleanly when input closes");
}

}  // namespace

int main() {
  testMatchReplayAndQuit();
  testEndOfFileExitsCleanly();
  return failures == 0 ? 0 : 1;
}
