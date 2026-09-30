# Command-Line Blackjack

A basic single-player C++ blackjack game with ASCII cards, automatic scoring,
and an automated dealer.

## Build and run

```bash
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp blackjack.cpp -o app
./app
```

Run the automated logic tests with:

```bash
./test_runner.sh
```

## Rules

- A session begins with `$1,000.00`.
- Enter a whole-dollar wager from `$1` through the available balance.
- Every round uses a freshly shuffled 52-card deck.
- The player may hit or stand. Hand totals are calculated automatically.
- Aces count as 11 when possible and otherwise count as 1.
- The dealer's second card remains hidden until the player stands or has a
  blackjack. The dealer does not play after a player bust.
- The dealer hits on 16 or below and stands on all 17s, including soft 17.
- Standard wins pay 1:1, pushes return the wager, and natural blackjacks pay
  3:2.
- Reaching `$0.00` ends the game.

This intentionally basic version does not include splitting, doubling down,
insurance, surrender, or multi-deck shoes.
