#include "guess_my_number.hpp"

#if defined(__APPLE__)
#include <mach-o/dyld.h>
#elif defined(__linux__)
#include <unistd.h>
#endif

#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <optional>
#include <random>
#include <string>
#include <vector>

namespace {

std::optional<std::filesystem::path> executablePath() {
#if defined(__APPLE__)
  std::uint32_t bufferSize = 0;
  if (_NSGetExecutablePath(nullptr, &bufferSize) != -1 || bufferSize == 0) {
    return std::nullopt;
  }

  std::vector<char> buffer(bufferSize);
  if (_NSGetExecutablePath(buffer.data(), &bufferSize) != 0) {
    return std::nullopt;
  }
  return buffer.data();
#elif defined(__linux__)
  std::vector<char> buffer(256);
  while (true) {
    const ssize_t length = readlink("/proc/self/exe", buffer.data(),
                                    buffer.size());
    if (length < 0) {
      return std::nullopt;
    }
    if (static_cast<std::size_t>(length) < buffer.size()) {
      return std::string(buffer.data(), length);
    }
    buffer.resize(buffer.size() * 2);
  }
#else
  return std::nullopt;
#endif
}

std::filesystem::path scorePathForExecutable() {
  const std::optional<std::filesystem::path> path = executablePath();
  if (!path) {
    std::cerr << "Warning: could not resolve executable path; using the current "
                 "directory for the high score.\n";
    return std::filesystem::current_path() / "high_score.txt";
  }

  std::error_code error;
  const std::filesystem::path executablePath =
      std::filesystem::weakly_canonical(*path, error);
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
