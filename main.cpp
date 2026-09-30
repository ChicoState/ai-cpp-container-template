#include <cmath>
#include <iostream>
#include <sstream>
#include <string>

namespace {

bool parseFiniteNumber(const std::string& token, double& value) {
  try {
    std::size_t parsedCharacters = 0;
    value = std::stod(token, &parsedCharacters);
    return parsedCharacters == token.size() && std::isfinite(value);
  } catch (const std::exception&) {
    return false;
  }
}

}  // namespace

int main() {
  std::string line;
  if (!std::getline(std::cin, line)) {
    std::cerr << "Incomplete input.\n";
    return 1;
  }

  std::istringstream input(line);
  std::string firstToken;
  std::string operatorToken;
  std::string secondToken;
  std::string extraToken;

  if (!(input >> firstToken >> operatorToken >> secondToken) || input >> extraToken) {
    std::cerr << "Invalid input. Enter: number operator number.\n";
    return 1;
  }

  double firstOperand{};
  double secondOperand{};
  if (!parseFiniteNumber(firstToken, firstOperand) ||
      !parseFiniteNumber(secondToken, secondOperand)) {
    std::cerr << "Invalid numeric operand.\n";
    return 1;
  }

  if (operatorToken.size() != 1) {
    std::cerr << "Unsupported operator.\n";
    return 1;
  }

  const char operation = operatorToken.front();
  double result{};

  switch (operation) {
    case '+':
      result = firstOperand + secondOperand;
      break;
    case '-':
      result = firstOperand - secondOperand;
      break;
    case '*':
      result = firstOperand * secondOperand;
      break;
    case '/':
      if (secondOperand == 0.0) {
        std::cerr << "Division by zero is not allowed.\n";
        return 1;
      }
      result = firstOperand / secondOperand;
      break;
    default:
      std::cerr << "Unsupported operator.\n";
      return 1;
  }

  std::cout << result << '\n';
  return 0;
}
