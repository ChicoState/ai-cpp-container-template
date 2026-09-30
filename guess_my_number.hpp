#ifndef GUESS_MY_NUMBER_HPP
#define GUESS_MY_NUMBER_HPP

#include <filesystem>
#include <functional>
#include <istream>
#include <optional>
#include <ostream>
#include <string>

namespace guess_my_number {

enum class GuessResult { tooLow, tooHigh, correct };

enum class HighScoreStatus { missing, valid, malformed, unreadable };

struct HighScoreLoadResult {
  HighScoreStatus status;
  std::optional<int> value;
  std::string diagnostic;
};

using TargetProvider = std::function<int()>;

std::optional<int> parseGuess(const std::string& input);
GuessResult evaluateGuess(int guess, int target);
HighScoreLoadResult loadHighScore(const std::filesystem::path& scorePath);
bool shouldSaveScore(const HighScoreLoadResult& currentScore, int attempts);
bool saveHighScore(const std::filesystem::path& scorePath, int score,
                   std::string& error);
int runGame(std::istream& input, std::ostream& output,
            TargetProvider targetProvider,
            const std::filesystem::path& scorePath);

}  // namespace guess_my_number

#endif
