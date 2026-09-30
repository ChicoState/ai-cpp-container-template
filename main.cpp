#include "blackjack.hpp"

#include <cctype>
#include <iomanip>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace {

using blackjack::Card;
using blackjack::Hand;
using blackjack::Money;
using blackjack::Outcome;
using blackjack::Rank;
using blackjack::Suit;

constexpr Money kStartingBalance = 100000;

std::string rankLabel(Rank rank) {
  switch (rank) {
    case Rank::Two: return "2";
    case Rank::Three: return "3";
    case Rank::Four: return "4";
    case Rank::Five: return "5";
    case Rank::Six: return "6";
    case Rank::Seven: return "7";
    case Rank::Eight: return "8";
    case Rank::Nine: return "9";
    case Rank::Ten: return "10";
    case Rank::Jack: return "J";
    case Rank::Queen: return "Q";
    case Rank::King: return "K";
    case Rank::Ace: return "A";
  }
  return "?";
}

std::string suitLabel(Suit suit) {
  switch (suit) {
    case Suit::Clubs: return "C";
    case Suit::Diamonds: return "D";
    case Suit::Hearts: return "H";
    case Suit::Spades: return "S";
  }
  return "?";
}

std::vector<std::string> cardArt(const Card& card) {
  const std::string rank = rankLabel(card.rank);
  const std::string left = rank + std::string(5 - rank.size(), ' ');
  const std::string right = std::string(5 - rank.size(), ' ') + rank;
  return {"+-----+", "|" + left + "|", "|  " + suitLabel(card.suit) + "  |",
          "|" + right + "|", "+-----+"};
}

std::vector<std::string> hiddenCardArt() {
  return {"+-----+", "|#####|", "|#####|", "|#####|", "+-----+"};
}

void printCards(const std::vector<Card>& cards, bool hideSecondCard) {
  std::vector<std::vector<std::string>> cardsToPrint;
  for (std::size_t index = 0; index < cards.size(); ++index) {
    cardsToPrint.push_back(hideSecondCard && index == 1 ? hiddenCardArt()
                                                        : cardArt(cards[index]));
  }

  for (std::size_t line = 0; line < 5; ++line) {
    for (const auto& card : cardsToPrint) {
      std::cout << card[line] << ' ';
    }
    std::cout << '\n';
  }
}

void printHand(const std::string& name, const Hand& hand, bool hideSecondCard) {
  std::cout << '\n' << name << ":\n";
  printCards(hand.cards(), hideSecondCard);
  if (!hideSecondCard) {
    std::cout << name << " total: " << hand.value() << '\n';
  }
}

std::string formatMoney(Money cents) {
  std::ostringstream output;
  output << '$' << cents / 100 << '.' << std::setw(2) << std::setfill('0')
         << cents % 100;
  return output.str();
}

std::optional<Money> readWager(Money balance) {
  while (true) {
    std::cout << "Balance: " << formatMoney(balance)
              << ". Enter a whole-dollar wager: ";
    std::string input;
    if (!std::getline(std::cin, input)) {
      return std::nullopt;
    }

    std::istringstream parser(input);
    Money dollars = 0;
    std::string extra;
    if (!(parser >> dollars) || (parser >> extra) || dollars < 1 ||
        dollars > balance / 100) {
      std::cout << "Enter a whole-dollar wager between $1 and "
                << formatMoney(balance) << ".\n";
      continue;
    }
    const Money wager = dollars * 100;
    if (!blackjack::isValidWager(balance, wager)) {
      std::cout << "Enter a whole-dollar wager between $1 and "
                << formatMoney(balance) << ".\n";
      continue;
    }
    return wager;
  }
}

std::optional<char> readPlayerAction() {
  while (true) {
    std::cout << "Hit or stand? [h/s]: ";
    std::string input;
    if (!std::getline(std::cin, input)) {
      return std::nullopt;
    }

    std::istringstream parser(input);
    char action = '\0';
    std::string extra;
    if ((parser >> action) && !(parser >> extra)) {
      action = static_cast<char>(std::tolower(static_cast<unsigned char>(action)));
      if (action == 'h' || action == 's') {
        return action;
      }
    }
    std::cout << "Please enter h to hit or s to stand.\n";
  }
}

std::optional<bool> readPlayAgain() {
  while (true) {
    std::cout << "Play another round? [y/n]: ";
    std::string input;
    if (!std::getline(std::cin, input)) {
      return std::nullopt;
    }

    std::istringstream parser(input);
    char answer = '\0';
    std::string extra;
    if ((parser >> answer) && !(parser >> extra)) {
      answer = static_cast<char>(std::tolower(static_cast<unsigned char>(answer)));
      if (answer == 'y') {
        return true;
      }
      if (answer == 'n') {
        return false;
      }
    }
    std::cout << "Please enter y or n.\n";
  }
}

void printOutcome(Outcome outcome) {
  switch (outcome) {
    case Outcome::PlayerWin:
      std::cout << "You win!\n";
      break;
    case Outcome::PlayerBlackjack:
      std::cout << "Blackjack! You win 3:2.\n";
      break;
    case Outcome::Push:
      std::cout << "Push. Your wager is returned.\n";
      break;
    case Outcome::DealerWin:
      std::cout << "Dealer wins.\n";
      break;
  }
}

std::optional<Outcome> playRound(blackjack::Deck& deck, Hand& player,
                                 Hand& dealer) {
  player.add(deck.draw());
  dealer.add(deck.draw());
  player.add(deck.draw());
  dealer.add(deck.draw());

  printHand("Dealer", dealer, true);
  printHand("Player", player, false);

  if (player.isBlackjack()) {
    printHand("Dealer", dealer, false);
    return dealer.isBlackjack() ? Outcome::Push : Outcome::PlayerBlackjack;
  }

  while (true) {
    const std::optional<char> action = readPlayerAction();
    if (!action) {
      return std::nullopt;
    }
    if (*action == 's') {
      break;
    }

    player.add(deck.draw());
    printHand("Player", player, false);
    if (player.isBust()) {
      std::cout << "You bust. Dealer wins.\n";
      return Outcome::DealerWin;
    }
  }

  printHand("Dealer", dealer, false);
  if (dealer.isBlackjack()) {
    return Outcome::DealerWin;
  }

  while (blackjack::dealerAction(dealer) == blackjack::DealerAction::Hit) {
    dealer.add(deck.draw());
    std::cout << "Dealer hits.\n";
    printHand("Dealer", dealer, false);
  }

  if (dealer.isBust()) {
    std::cout << "Dealer busts.\n";
    return Outcome::PlayerWin;
  }

  std::cout << "Dealer stands.\n";
  if (player.value() > dealer.value()) {
    return Outcome::PlayerWin;
  }
  if (player.value() < dealer.value()) {
    return Outcome::DealerWin;
  }
  return Outcome::Push;
}

}  // namespace

int main() {
  Money balance = kStartingBalance;
  std::cout << "Welcome to Blackjack!\n";

  while (balance > 0) {
    const std::optional<Money> wager = readWager(balance);
    if (!wager) {
      std::cout << "Thanks for playing.\n";
      return 0;
    }
    balance -= *wager;

    blackjack::Deck deck;
    Hand player;
    Hand dealer;
    const std::optional<Outcome> outcome = playRound(deck, player, dealer);
    if (!outcome) {
      std::cout << "Thanks for playing.\n";
      return 0;
    }

    balance = blackjack::settleBalance(balance, *wager, *outcome);
    printOutcome(*outcome);
    std::cout << "Balance: " << formatMoney(balance) << '\n';

    if (balance == 0) {
      std::cout << "You are out of money. Game over.\n";
      return 0;
    }

    const std::optional<bool> playAgain = readPlayAgain();
    if (!playAgain || !*playAgain) {
      std::cout << "Thanks for playing.\n";
      return 0;
    }
  }
}
