#include "guess_my_number.hpp"

#include <cctype>
#include <fstream>
#include <limits>

namespace guess_my_number {
namespace {

std::string trimAsciiWhitespace(const std::string& input) {
  std::size_t start = 0;
  while (start < input.size() &&
         std::isspace(static_cast<unsigned char>(input[start]))) {
    ++start;
  }

  std::size_t end = input.size();
  while (end > start &&
         std::isspace(static_cast<unsigned char>(input[end - 1]))) {
    --end;
  }

  return input.substr(start, end - start);
}

std::optional<int> parseWholeNumber(const std::string& input,
                                    bool allowSign) {
  const std::string trimmed = trimAsciiWhitespace(input);
  if (trimmed.empty()) {
    return std::nullopt;
  }

  std::size_t index = 0;
  bool negative = false;
  if (allowSign && (trimmed[0] == '+' || trimmed[0] == '-')) {
    negative = trimmed[0] == '-';
    ++index;
  }

  if (index == trimmed.size()) {
    return std::nullopt;
  }

  int value = 0;
  for (; index < trimmed.size(); ++index) {
    const unsigned char character = static_cast<unsigned char>(trimmed[index]);
    if (!std::isdigit(character)) {
      return std::nullopt;
    }

    const int digit = character - '0';
    if (value > (std::numeric_limits<int>::max() - digit) / 10) {
      return std::nullopt;
    }
    value = value * 10 + digit;
  }

  return negative ? -value : value;
}

bool isReplayAnswer(const std::string& input) {
  const std::string trimmed = trimAsciiWhitespace(input);
  return trimmed == "y" || trimmed == "Y" || trimmed == "yes" ||
         trimmed == "YES" || trimmed == "Yes";
}

bool isExitAnswer(const std::string& input) {
  const std::string trimmed = trimAsciiWhitespace(input);
  return trimmed == "n" || trimmed == "N" || trimmed == "no" ||
         trimmed == "NO" || trimmed == "No";
}

void reportHighScoreStatus(const HighScoreLoadResult& score,
                           std::ostream& output) {
  if (score.status == HighScoreStatus::valid) {
    output << "High score: " << *score.value << " valid guesses.\n";
  } else if (score.status == HighScoreStatus::malformed ||
             score.status == HighScoreStatus::unreadable) {
    output << "Warning: " << score.diagnostic << '\n';
  }
}

}  // namespace

std::optional<int> parseGuess(const std::string& input) {
  const std::optional<int> parsed = parseWholeNumber(input, true);
  if (!parsed || *parsed < 0 || *parsed > 100) {
    return std::nullopt;
  }
  return parsed;
}

GuessResult evaluateGuess(int guess, int target) {
  if (guess < target) {
    return GuessResult::tooLow;
  }
  if (guess > target) {
    return GuessResult::tooHigh;
  }
  return GuessResult::correct;
}

HighScoreLoadResult loadHighScore(const std::filesystem::path& scorePath) {
  std::error_code error;
  if (!std::filesystem::exists(scorePath, error)) {
    if (error) {
      return {HighScoreStatus::unreadable, std::nullopt,
              "could not inspect the high score file"};
    }
    return {HighScoreStatus::missing, std::nullopt, ""};
  }

  if (!std::filesystem::is_regular_file(scorePath, error) || error) {
    return {HighScoreStatus::unreadable, std::nullopt,
            "could not read the high score file; it will be preserved"};
  }

  std::ifstream scoreFile(scorePath);
  if (!scoreFile) {
    return {HighScoreStatus::unreadable, std::nullopt,
            "could not read the high score file; it will be preserved"};
  }

  std::string contents;
  std::getline(scoreFile, contents, '\0');
  if (scoreFile.bad()) {
    return {HighScoreStatus::unreadable, std::nullopt,
            "could not read the high score file; it will be preserved"};
  }

  const std::optional<int> score = parseWholeNumber(contents, false);
  if (!score || *score < 1) {
    return {HighScoreStatus::malformed, std::nullopt,
            "high score file is malformed; it will be preserved"};
  }

  return {HighScoreStatus::valid, score, ""};
}

bool shouldSaveScore(const HighScoreLoadResult& currentScore, int attempts) {
  if (attempts < 1) {
    return false;
  }
  if (currentScore.status == HighScoreStatus::missing) {
    return true;
  }
  return currentScore.status == HighScoreStatus::valid &&
         attempts < *currentScore.value;
}

bool saveHighScore(const std::filesystem::path& scorePath, int score,
                   std::string& error) {
  std::ofstream scoreFile(scorePath, std::ios::trunc);
  if (!scoreFile) {
    error = "could not write the high score file";
    return false;
  }

  scoreFile << score << '\n';
  if (!scoreFile) {
    error = "could not write the high score file";
    return false;
  }
  return true;
}

int runGame(std::istream& input, std::ostream& output,
            TargetProvider targetProvider,
            const std::filesystem::path& scorePath) {
  HighScoreLoadResult highScore = loadHighScore(scorePath);
  reportHighScoreStatus(highScore, output);

  while (true) {
    const int target = targetProvider();
    int attempts = 0;

    while (true) {
      output << "Guess a number from 0 to 100: ";
      std::string line;
      if (!std::getline(input, line)) {
        output << "Goodbye!\n";
        return 0;
      }

      const std::optional<int> guess = parseGuess(line);
      if (!guess) {
        output << "Invalid guess. Enter a whole number from 0 to 100.\n";
        continue;
      }

      ++attempts;
      switch (evaluateGuess(*guess, target)) {
        case GuessResult::tooLow:
          output << "Higher\n";
          break;
        case GuessResult::tooHigh:
          output << "Lower\n";
          break;
        case GuessResult::correct:
          output << "Correct! You won in " << attempts << " valid guesses.\n";
          break;
      }

      if (evaluateGuess(*guess, target) == GuessResult::correct) {
        break;
      }
    }

    if (shouldSaveScore(highScore, attempts)) {
      std::string error;
      if (saveHighScore(scorePath, attempts, error)) {
        output << "New high score: " << attempts << " valid guesses.\n";
        highScore = {HighScoreStatus::valid, attempts, ""};
      } else {
        output << "Warning: " << error << '\n';
      }
    }

    while (true) {
      output << "Play again? (y/n): ";
      std::string response;
      if (!std::getline(input, response)) {
        output << "Goodbye!\n";
        return 0;
      }
      if (isReplayAnswer(response)) {
        break;
      }
      if (isExitAnswer(response)) {
        output << "Goodbye!\n";
        return 0;
      }
      output << "Please enter y or n.\n";
    }
  }
}

}  // namespace guess_my_number
