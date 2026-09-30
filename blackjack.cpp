#include "blackjack.hpp"

#include <algorithm>
#include <random>
#include <stdexcept>

namespace blackjack {

namespace {

std::mt19937& randomEngine() {
  static std::mt19937 engine(std::random_device{}());
  return engine;
}

}  // namespace

Deck::Deck() {
  for (int suit = static_cast<int>(Suit::Clubs);
       suit <= static_cast<int>(Suit::Spades); ++suit) {
    for (int rank = static_cast<int>(Rank::Two);
         rank <= static_cast<int>(Rank::Ace); ++rank) {
      cards_.push_back(
          {static_cast<Rank>(rank), static_cast<Suit>(suit)});
    }
  }
  shuffle();
}

void Deck::shuffle() {
  std::shuffle(cards_.begin(), cards_.end(), randomEngine());
}

Card Deck::draw() {
  if (cards_.empty()) {
    throw std::out_of_range("Cannot draw from an empty deck.");
  }

  const Card nextCard = cards_.back();
  cards_.pop_back();
  return nextCard;
}

std::size_t Deck::size() const {
  return cards_.size();
}

void Hand::add(Card card) {
  cards_.push_back(card);
}

int Hand::value() const {
  int total = 0;
  int aceCount = 0;

  for (const Card& card : cards_) {
    total += cardValue(card.rank);
    if (card.rank == Rank::Ace) {
      ++aceCount;
    }
  }

  while (total > 21 && aceCount > 0) {
    total -= 10;
    --aceCount;
  }

  return total;
}

bool Hand::isSoft() const {
  int total = 0;
  int aceCount = 0;

  for (const Card& card : cards_) {
    total += cardValue(card.rank);
    if (card.rank == Rank::Ace) {
      ++aceCount;
    }
  }

  while (total > 21 && aceCount > 0) {
    total -= 10;
    --aceCount;
  }

  return aceCount > 0;
}

bool Hand::isBlackjack() const {
  return cards_.size() == 2 && value() == 21;
}

bool Hand::isBust() const {
  return value() > 21;
}

const std::vector<Card>& Hand::cards() const {
  return cards_;
}

DealerAction dealerAction(const Hand& hand) {
  return hand.value() <= 16 ? DealerAction::Hit : DealerAction::Stand;
}

Money settleBalance(Money balanceAfterWager, Money wager, Outcome outcome) {
  switch (outcome) {
    case Outcome::PlayerWin:
      return balanceAfterWager + wager * 2;
    case Outcome::PlayerBlackjack:
      return balanceAfterWager + wager * 5 / 2;
    case Outcome::Push:
      return balanceAfterWager + wager;
    case Outcome::DealerWin:
      return balanceAfterWager;
  }

  return balanceAfterWager;
}

bool isValidWager(Money balance, Money wager) {
  return balance > 0 && wager >= 100 && wager <= balance && wager % 100 == 0;
}

int cardValue(Rank rank) {
  if (rank == Rank::Ace) {
    return 11;
  }
  if (rank == Rank::Jack || rank == Rank::Queen || rank == Rank::King) {
    return 10;
  }
  return static_cast<int>(rank);
}

}  // namespace blackjack
