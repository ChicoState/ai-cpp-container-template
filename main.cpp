#include "guess_my_number.hpp"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <mach-o/dyld.h>
#include <random>
#include <string>
#include <vector>

namespace {

std::filesystem::path scorePathForExecutable() {
  std::uint32_t bufferSize = 0;
  if (_NSGetExecutablePath(nullptr, &bufferSize) != -1 || bufferSize == 0) {
    std::cerr << "Warning: could not resolve executable path; using the current "
                 "directory for the high score.\n";
    return std::filesystem::current_path() / "high_score.txt";
  }

  std::vector<char> buffer(bufferSize);
  if (_NSGetExecutablePath(buffer.data(), &bufferSize) != 0) {
    std::cerr << "Warning: could not resolve executable path; using the current "
                 "directory for the high score.\n";
    return std::filesystem::current_path() / "high_score.txt";
  }

  std::error_code error;
  const std::filesystem::path executablePath =
      std::filesystem::weakly_canonical(buffer.data(), error);
  if (error) {
    std::cerr << "Warning: could not resolve executable path; using the current "
                 "directory for the high score.\n";
    return std::filesystem::current_path() / "high_score.txt";
  }

  return executablePath.parent_path() / "high_score.txt";
}

guess_my_number::TargetProvider makeTargetProvider() {
#ifdef GUESS_MY_NUMBER_TESTING
  if (const char* configuredTarget = std::getenv("GUESS_MY_NUMBER_TARGET")) {
    const std::optional<int> target = guess_my_number::parseGuess(configuredTarget);
    if (target) {
      return [value = *target] { return value; };
    }
  }
#endif

  return [engine = std::mt19937(std::random_device{}()),
          distribution = std::uniform_int_distribution<int>(0, 100)]() mutable {
    return distribution(engine);
  };
}

}  // namespace

int main() {
  return guess_my_number::runGame(std::cin, std::cout, makeTargetProvider(),
                                  scorePathForExecutable());
}
