#include "../blackjack.hpp"

#include <cassert>
#include <iostream>

using blackjack::Card;
using blackjack::DealerAction;
using blackjack::Hand;
using blackjack::Outcome;
using blackjack::Rank;
using blackjack::Suit;

namespace {

Card card(Rank rank) {
  return {rank, Suit::Hearts};
}

void testAceScoring() {
  Hand hand;
  hand.add(card(Rank::Ace));
  hand.add(card(Rank::Six));
  assert(hand.value() == 17);
  assert(hand.isSoft());

  hand.add(card(Rank::King));
  assert(hand.value() == 17);
  assert(!hand.isSoft());
}

void testBlackjackAndBust() {
  Hand blackjack;
  blackjack.add(card(Rank::Ace));
  blackjack.add(card(Rank::Queen));
  assert(blackjack.isBlackjack());
  assert(!blackjack.isBust());

  Hand bust;
  bust.add(card(Rank::King));
  bust.add(card(Rank::Queen));
  bust.add(card(Rank::Two));
  assert(bust.isBust());
}

void testDealerPolicy() {
  Hand sixteen;
  sixteen.add(card(Rank::Ten));
  sixteen.add(card(Rank::Six));
  assert(blackjack::dealerAction(sixteen) == DealerAction::Hit);

  Hand softSeventeen;
  softSeventeen.add(card(Rank::Ace));
  softSeventeen.add(card(Rank::Six));
  assert(blackjack::dealerAction(softSeventeen) == DealerAction::Stand);
}

void testPayouts() {
  constexpr blackjack::Money startingBalance = 100000;
  constexpr blackjack::Money wager = 1000;
  const blackjack::Money balanceAfterWager = startingBalance - wager;

  assert(blackjack::settleBalance(balanceAfterWager, wager, Outcome::PlayerWin) ==
         101000);
  assert(blackjack::settleBalance(balanceAfterWager, wager,
                                  Outcome::PlayerBlackjack) == 101500);
  assert(blackjack::settleBalance(balanceAfterWager, wager, Outcome::Push) ==
         startingBalance);
  assert(blackjack::settleBalance(balanceAfterWager, wager, Outcome::DealerWin) ==
         balanceAfterWager);
}

void testWagerValidation() {
  constexpr blackjack::Money balance = 100000;
  assert(blackjack::isValidWager(balance, 100));
  assert(blackjack::isValidWager(balance, balance));
  assert(!blackjack::isValidWager(balance, 0));
  assert(!blackjack::isValidWager(balance, 50));
  assert(!blackjack::isValidWager(balance, balance + 100));
  assert(!blackjack::isValidWager(0, 100));
}

void testDeckContainsUniqueCards() {
  blackjack::Deck deck;
  assert(deck.size() == 52);

  bool seen[52] = {};
  for (int index = 0; index < 52; ++index) {
    const Card drawn = deck.draw();
    const int cardIndex = static_cast<int>(drawn.suit) * 13 +
                          static_cast<int>(drawn.rank) -
                          static_cast<int>(Rank::Two);
    assert(!seen[cardIndex]);
    seen[cardIndex] = true;
  }
  assert(deck.size() == 0);
}

}  // namespace

int main() {
  testAceScoring();
  testBlackjackAndBust();
  testDealerPolicy();
  testPayouts();
  testWagerValidation();
  testDeckContainsUniqueCards();
  std::cout << "All blackjack logic tests passed.\n";
}
