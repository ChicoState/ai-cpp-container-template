#include "game.h"

#include <cctype>
#include <iomanip>
#include <sstream>

namespace {

constexpr char kWall = '#';
constexpr char kKey = 'K';
constexpr char kExit = 'E';

}  // namespace

Game::Game()
    : terrain_({
          "#########",
          "#.....K.#",
          "#.#####.#",
          "#.......#",
          "#.#####.#",
          "#.......#",
          "#.#####E#",
          "#.......#",
          "#########",
      }),
      hazardRoute_({{5, 4}, {5, 5}, {5, 6}, {5, 7},
                    {5, 6}, {5, 5}, {5, 4}, {5, 3}}),
      lastMessage_("Find the key, avoid the hazard, and reach the exit.") {}

void Game::applyInput(char input, Clock::time_point now) {
  if (status_ != GameStatus::Active) {
    return;
  }

  const char command = static_cast<char>(
      std::tolower(static_cast<unsigned char>(input)));
  if (command == 'q') {
    quit(now);
    return;
  }

  Position next = player_;
  switch (command) {
    case 'w':
      --next.row;
      break;
    case 'a':
      --next.column;
      break;
    case 's':
      ++next.row;
      break;
    case 'd':
      ++next.column;
      break;
    default:
      lastMessage_ = "Use W, A, S, D to move or Q to quit.";
      return;
  }

  if (!isWalkable(next)) {
    lastMessage_ = "A wall blocks that path.";
    return;
  }

  player_ = next;
  if (!startedAt_.has_value()) {
    startedAt_ = now;
  }

  if (player_ == hazard_) {
    finish(GameStatus::Lost, now);
    return;
  }

  const char tile = terrain_[static_cast<std::size_t>(player_.row)]
                            [static_cast<std::size_t>(player_.column)];
  if (tile == kKey) {
    hasKey_ = true;
    lastMessage_ = "Key collected. The exit is unlocked.";
  }

  if (tile == kExit) {
    if (hasKey_) {
      finish(GameStatus::Won, now);
      return;
    }
    lastMessage_ = "The exit is locked. Find the key first.";
  }

  moveHazard();
  if (hazard_ == player_) {
    finish(GameStatus::Lost, now);
  }
}

void Game::quit(Clock::time_point now) {
  if (status_ == GameStatus::Active) {
    finish(GameStatus::Quit, now);
  }
}

Position Game::playerPosition() const { return player_; }

Position Game::hazardPosition() const { return hazard_; }

GameStatus Game::status() const { return status_; }

bool Game::hasKey() const { return hasKey_; }

std::chrono::milliseconds Game::elapsed(Clock::time_point now) const {
  if (!startedAt_.has_value()) {
    return std::chrono::milliseconds::zero();
  }

  const Clock::time_point end = finishedAt_.value_or(now);
  return std::chrono::duration_cast<std::chrono::milliseconds>(end - *startedAt_);
}

std::string Game::render(Clock::time_point now) const {
  std::vector<std::string> frame = terrain_;
  if (hasKey_) {
    for (std::string& row : frame) {
      for (char& tile : row) {
        if (tile == kKey) {
          tile = '.';
        }
      }
    }
  }

  frame[static_cast<std::size_t>(hazard_.row)]
       [static_cast<std::size_t>(hazard_.column)] = 'H';
  frame[static_cast<std::size_t>(player_.row)]
       [static_cast<std::size_t>(player_.column)] = 'P';

  std::ostringstream output;
  output << "Time: " << formatDuration(elapsed(now))
         << " | Key: " << (hasKey_ ? "yes" : "no") << '\n';
  for (const std::string& row : frame) {
    output << row << '\n';
  }
  return output.str();
}

const std::string& Game::lastMessage() const { return lastMessage_; }

bool Game::isWalkable(Position position) const {
  if (position.row < 0 || position.column < 0 ||
      position.row >= static_cast<int>(terrain_.size()) ||
      position.column >= static_cast<int>(terrain_.front().size())) {
    return false;
  }

  return terrain_[static_cast<std::size_t>(position.row)]
                 [static_cast<std::size_t>(position.column)] != kWall;
}

void Game::finish(GameStatus status, Clock::time_point now) {
  status_ = status;
  if (startedAt_.has_value()) {
    finishedAt_ = now;
  }

  switch (status) {
    case GameStatus::Won:
      lastMessage_ = "You escaped the maze!";
      break;
    case GameStatus::Lost:
      lastMessage_ = "The hazard caught you.";
      break;
    case GameStatus::Quit:
      lastMessage_ = "Run ended. Thanks for playing.";
      break;
    case GameStatus::Active:
      break;
  }
}

void Game::moveHazard() {
  hazardRouteIndex_ = (hazardRouteIndex_ + 1) % hazardRoute_.size();
  hazard_ = hazardRoute_[hazardRouteIndex_];
}

std::string formatDuration(std::chrono::milliseconds duration) {
  const long long totalCentiseconds = duration.count() / 10;
  const long long centiseconds = totalCentiseconds % 100;
  const long long totalSeconds = totalCentiseconds / 100;
  const long long seconds = totalSeconds % 60;
  const long long minutes = totalSeconds / 60;

  std::ostringstream output;
  output << std::setfill('0') << std::setw(2) << minutes << ':'
         << std::setw(2) << seconds << '.' << std::setw(2) << centiseconds;
  return output.str();
}
