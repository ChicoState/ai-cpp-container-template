#ifndef MAZEGAME_GAME_H
#define MAZEGAME_GAME_H

#include <chrono>
#include <optional>
#include <string>
#include <vector>

struct Position {
  int row;
  int column;

  bool operator==(const Position& other) const {
    return row == other.row && column == other.column;
  }
};

enum class GameStatus { Active, Won, Lost, Quit };

class Game {
 public:
  using Clock = std::chrono::steady_clock;

  Game();

  void applyInput(char input, Clock::time_point now);
  void quit(Clock::time_point now);

  Position playerPosition() const;
  Position hazardPosition() const;
  GameStatus status() const;
  bool hasKey() const;
  std::chrono::milliseconds elapsed(Clock::time_point now) const;
  std::string render(Clock::time_point now) const;
  const std::string& lastMessage() const;

 private:
  bool isWalkable(Position position) const;
  void finish(GameStatus status, Clock::time_point now);
  void moveHazard();

  std::vector<std::string> terrain_;
  std::vector<Position> hazardRoute_;
  Position player_{1, 1};
  Position hazard_{5, 4};
  std::size_t hazardRouteIndex_ = 0;
  bool hasKey_ = false;
  GameStatus status_ = GameStatus::Active;
  std::optional<Clock::time_point> startedAt_;
  std::optional<Clock::time_point> finishedAt_;
  std::string lastMessage_;
};

std::string formatDuration(std::chrono::milliseconds duration);

#endif
