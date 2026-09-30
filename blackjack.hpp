#ifndef BLACKJACK_HPP
#define BLACKJACK_HPP

#include <cstddef>
#include <vector>

namespace blackjack {

using Money = long long;

enum class Suit { Clubs, Diamonds, Hearts, Spades };

enum class Rank {
  Two = 2,
  Three,
  Four,
  Five,
  Six,
  Seven,
  Eight,
  Nine,
  Ten,
  Jack,
  Queen,
  King,
  Ace
};

struct Card {
  Rank rank;
  Suit suit;
};

class Deck {
 public:
  Deck();

  void shuffle();
  Card draw();
  std::size_t size() const;

 private:
  std::vector<Card> cards_;
};

class Hand {
 public:
  void add(Card card);
  int value() const;
  bool isSoft() const;
  bool isBlackjack() const;
  bool isBust() const;
  const std::vector<Card>& cards() const;

 private:
  std::vector<Card> cards_;
};

enum class DealerAction { Hit, Stand };
enum class Outcome { PlayerWin, DealerWin, Push, PlayerBlackjack };

DealerAction dealerAction(const Hand& hand);
Money settleBalance(Money balanceAfterWager, Money wager, Outcome outcome);
bool isValidWager(Money balance, Money wager);
int cardValue(Rank rank);

}  // namespace blackjack

#endif
