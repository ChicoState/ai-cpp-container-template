#include <iostream>
#include <limits>
#include <sstream>
#include <string>

namespace {

constexpr char k_invalid_command_message[] =
    "Error: expected 'add <integer> <integer>', 'sub <integer> <integer>', "
    "or 'exit'.";

bool parse_integer(const std::string& token, int& value) {
  std::size_t parsed_characters = 0;

  try {
    const long long parsed_value = std::stoll(token, &parsed_characters);
    if (parsed_characters != token.size() ||
        parsed_value < std::numeric_limits<int>::min() ||
        parsed_value > std::numeric_limits<int>::max()) {
      return false;
    }

    value = static_cast<int>(parsed_value);
    return true;
  } catch (const std::exception&) {
    return false;
  }
}

bool has_only_exit_command(std::istringstream& input) {
  std::string extra_token;
  return !(input >> extra_token);
}

bool parse_operation(std::istringstream& input, int& left, int& right) {
  std::string left_token;
  std::string right_token;
  std::string extra_token;

  return (input >> left_token >> right_token) && !(input >> extra_token) &&
         parse_integer(left_token, left) && parse_integer(right_token, right);
}

void print_invalid_command() {
  std::cout << k_invalid_command_message << '\n';
}

}  // namespace

int main() {
  std::string line;

  while (std::getline(std::cin, line)) {
    std::istringstream input(line);
    std::string command;

    if (!(input >> command)) {
      print_invalid_command();
      continue;
    }

    if (command == "exit") {
      if (has_only_exit_command(input)) {
        return 0;
      }

      print_invalid_command();
      continue;
    }

    int left = 0;
    int right = 0;
    if ((command != "add" && command != "sub") ||
        !parse_operation(input, left, right)) {
      print_invalid_command();
      continue;
    }

    const long long result = command == "add"
                                 ? static_cast<long long>(left) + right
                                 : static_cast<long long>(left) - right;
    std::cout << result << '\n';
  }

  return 0;
}
