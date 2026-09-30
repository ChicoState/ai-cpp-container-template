#include "rps_game.hpp"

#include <algorithm>
#include <cctype>

namespace {

std::string normalizeInput(const std::string& input) {
  const auto first = std::find_if_not(input.begin(), input.end(),
                                     [](unsigned char character) {
                                       return std::isspace(character) != 0;
                                     });
  if (first == input.end()) {
    return "";
  }

  const auto last = std::find_if_not(input.rbegin(), input.rend(),
                                    [](unsigned char character) {
                                      return std::isspace(character) != 0;
                                    }).base();

  std::string normalized(first, last);
  std::transform(normalized.begin(), normalized.end(), normalized.begin(),
                 [](unsigned char character) {
                   return static_cast<char>(std::tolower(character));
                 });
  return normalized;
}

std::string centered(const std::string& text, std::size_t width) {
  const std::size_t leftPadding = (width - text.size()) / 2;
  return std::string(leftPadding, ' ') + text +
         std::string(width - leftPadding - text.size(), ' ');
}

const std::vector<std::string>& cardFor(Move move) {
  static const std::vector<std::string> rock = {
      "+-----------+", "|   ROCK    |", "|     O     |", "+-----------+"};
  static const std::vector<std::string> paper = {
      "+-----------+", "|   PAPER   |", "|   [___]   |", "+-----------+"};
  static const std::vector<std::string> scissors = {
      "+-----------+", "| SCISSORS  |", "|   X X     |", "+-----------+"};

  switch (move) {
    case Move::Rock:
      return rock;
    case Move::Paper:
      return paper;
    case Move::Scissors:
      return scissors;
  }

  return rock;
}

void writeRoundResult(std::ostream& output, RoundResult result) {
  if (result == RoundResult::PlayerWin) {
    output << "You win this round.\n";
  } else if (result == RoundResult::ComputerWin) {
    output << "Computer wins this round.\n";
  } else {
    output << "Round tied.";
  }
}

bool readMove(std::istream& input, std::ostream& output, Move& move) {
  std::string line;
  while (true) {
    output << "Choose [r]ock, [p]aper, or [s]cissors: ";
    if (!std::getline(input, line)) {
      return false;
    }

    const std::optional<Move> parsedMove = parseMove(line);
    if (parsedMove) {
      move = *parsedMove;
      return true;
    }

    output << "Invalid move. Enter r, p, or s.\n";
  }
}

}  // namespace

std::optional<Move> parseMove(const std::string& input) {
  const std::string normalized = normalizeInput(input);
  if (normalized == "r") {
    return Move::Rock;
  }
  if (normalized == "p") {
    return Move::Paper;
  }
  if (normalized == "s") {
    return Move::Scissors;
  }
  return std::nullopt;
}

RoundResult evaluateRound(Move playerMove, Move computerMove) {
  if (playerMove == computerMove) {
    return RoundResult::Tie;
  }

  if ((playerMove == Move::Rock && computerMove == Move::Scissors) ||
      (playerMove == Move::Paper && computerMove == Move::Rock) ||
      (playerMove == Move::Scissors && computerMove == Move::Paper)) {
    return RoundResult::PlayerWin;
  }

  return RoundResult::ComputerWin;
}

void applyRoundResult(Score& score, RoundResult result) {
  if (result == RoundResult::PlayerWin) {
    ++score.playerWins;
  } else if (result == RoundResult::ComputerWin) {
    ++score.computerWins;
  }
}

std::vector<std::string> renderRound(Move playerMove, Move computerMove) {
  const auto& playerCard = cardFor(playerMove);
  const auto& computerCard = cardFor(computerMove);
  std::vector<std::string> lines;
  lines.push_back(centered("Player", playerCard.front().size()) + " | " +
                  centered("Computer", computerCard.front().size()));

  for (std::size_t index = 0; index < playerCard.size(); ++index) {
    lines.push_back(playerCard[index] + " | " + computerCard[index]);
  }

  return lines;
}

void runGame(std::istream& input, std::ostream& output,
             const ComputerMoveProvider& nextComputerMove) {
  while (true) {
    Score score;
    output << "New match! First to two wins.\n";

    while (score.playerWins < 2 && score.computerWins < 2) {
      Move playerMove;
      if (!readMove(input, output, playerMove)) {
        output << "\nThanks for playing!\n";
        return;
      }

      const Move computerMove = nextComputerMove();
      for (const std::string& line : renderRound(playerMove, computerMove)) {
        output << line << '\n';
      }

      const RoundResult result = evaluateRound(playerMove, computerMove);
      applyRoundResult(score, result);
      writeRoundResult(output, result);
      output << " Score: Player " << score.playerWins << " - Computer "
             << score.computerWins << "\n";
    }

    if (score.playerWins == 2) {
      output << "You win the match!\n";
    } else {
      output << "Computer wins the match!\n";
    }

    output << "Play again? (y to continue): ";
    std::string replayResponse;
    if (!std::getline(input, replayResponse) ||
        normalizeInput(replayResponse) != "y") {
      output << "Thanks for playing!\n";
      return;
    }
  }
}
