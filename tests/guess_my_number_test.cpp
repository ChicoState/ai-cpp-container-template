#include "../guess_my_number.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace {

int failures = 0;

void expect(bool condition, const std::string& message) {
  if (!condition) {
    std::cerr << "FAIL: " << message << '\n';
    ++failures;
  }
}

std::filesystem::path makeTempDirectory() {
  const auto directory = std::filesystem::temp_directory_path() /
                         ("guess-my-number-test-" +
                          std::to_string(std::rand()));
  std::filesystem::create_directories(directory);
  return directory;
}

void testParseGuess() {
  using guess_my_number::parseGuess;

  expect(parseGuess("0") == 0, "accepts lower bound");
  expect(parseGuess("100") == 100, "accepts upper bound");
  expect(parseGuess("  +42\t") == 42,
         "trims whitespace and accepts a leading plus");
  expect(!parseGuess(""), "rejects blank input");
  expect(!parseGuess("42x"), "rejects partial numbers");
  expect(!parseGuess("-1"), "rejects negative numbers");
  expect(!parseGuess("101"), "rejects out-of-range numbers");
}

void testGuessEvaluation() {
  using guess_my_number::GuessResult;
  using guess_my_number::evaluateGuess;

  expect(evaluateGuess(41, 42) == GuessResult::tooLow,
         "identifies a low guess");
  expect(evaluateGuess(43, 42) == GuessResult::tooHigh,
         "identifies a high guess");
  expect(evaluateGuess(42, 42) == GuessResult::correct,
         "identifies a correct guess");
}

void testHighScorePersistence() {
  using guess_my_number::HighScoreStatus;
  using guess_my_number::loadHighScore;
  using guess_my_number::saveHighScore;
  using guess_my_number::shouldSaveScore;

  const auto directory = makeTempDirectory();
  const auto scorePath = directory / "high_score.txt";
  std::string error;

  auto missing = loadHighScore(scorePath);
  expect(missing.status == HighScoreStatus::missing,
         "reports a missing score file");
  expect(shouldSaveScore(missing, 5), "saves the first completed score");
  expect(saveHighScore(scorePath, 5, error), "writes the first score");

  auto valid = loadHighScore(scorePath);
  expect(valid.status == HighScoreStatus::valid && valid.value == 5,
         "loads a valid score");
  expect(shouldSaveScore(valid, 4), "replaces a better score");
  expect(!shouldSaveScore(valid, 5), "preserves an equal score");
  expect(!shouldSaveScore(valid, 6), "preserves a better existing score");

  {
    std::ofstream malformedFile(scorePath);
    malformedFile << "not-a-score\n";
  }
  auto malformed = loadHighScore(scorePath);
  expect(malformed.status == HighScoreStatus::malformed,
         "reports a malformed score file");
  expect(!shouldSaveScore(malformed, 1),
         "never overwrites a malformed score file");

  std::ifstream malformedFile(scorePath);
  std::string contents;
  std::getline(malformedFile, contents);
  expect(contents == "not-a-score", "preserves malformed score contents");

  auto unreadable = loadHighScore(directory);
  expect(unreadable.status == HighScoreStatus::unreadable,
         "reports a directory as an unreadable score path");

  std::string writeError;
  expect(!saveHighScore(directory, 1, writeError),
         "reports a failed score write to a directory");
  expect(!writeError.empty(), "includes a failed-write diagnostic");

  std::filesystem::remove_all(directory);
}

void testTerminalFlow() {
  using guess_my_number::runGame;

  const auto directory = makeTempDirectory();
  const auto scorePath = directory / "high_score.txt";
  std::istringstream input("oops\n101\n40\n42\nn\n");
  std::ostringstream output;
  int targetsProvided = 0;

  const int result = runGame(
      input, output,
      [&targetsProvided] {
        ++targetsProvided;
        return 42;
      },
      scorePath);

  const std::string transcript = output.str();
  expect(result == 0, "returns success after the user exits");
  expect(targetsProvided == 1, "uses one target for one completed round");
  expect(transcript.find("Invalid guess") != std::string::npos,
         "explains invalid guesses");
  expect(transcript.find("Higher") != std::string::npos,
         "gives higher feedback for a low guess");
  expect(transcript.find("2 valid guesses") != std::string::npos,
         "counts only valid guesses");
  expect(std::filesystem::exists(scorePath), "creates a score file after a win");

  std::istringstream eofInput;
  std::ostringstream eofOutput;
  expect(runGame(eofInput, eofOutput, [] { return 42; }, scorePath) == 0,
         "exits cleanly on EOF while guessing");

  const auto replayScorePath = directory / "replay_score.txt";
  std::istringstream replayEofInput("42\n");
  std::ostringstream replayEofOutput;
  expect(runGame(replayEofInput, replayEofOutput, [] { return 42; },
                 replayScorePath) == 0,
         "exits cleanly on EOF at the replay prompt");
  expect(replayEofOutput.str().find("Goodbye!") != std::string::npos,
         "reports a clean replay EOF exit");

  std::filesystem::remove_all(directory);
}

}  // namespace

int main() {
  testParseGuess();
  testGuessEvaluation();
  testHighScorePersistence();
  testTerminalFlow();

  if (failures != 0) {
    std::cerr << failures << " test(s) failed\n";
    return 1;
  }

  std::cout << "All Guess My Number tests passed\n";
  return 0;
}
