# Spec: Command-Line Blackjack

## Objective

Build a basic, single-player C++ command-line blackjack game for an educational
setting. The player should be able to see their cards in ASCII, place wagers
against a `$1,000.00` starting balance, make hit-or-stand decisions, and have
scores, dealer play, payouts, and game-over conditions handled automatically.

Success means a player can complete repeatable rounds using the agreed rules,
invalid input is safely rejected, and core game decisions are covered by
automated tests.

### User-facing rules

- The session begins with `$1,000.00`; money is stored as integer cents.
- The player enters a whole-dollar wager from `$1` through their balance.
  The wager is deducted before dealing.
- Every round uses a fresh, shuffled standard 52-card deck.
- Player cards are rendered side-by-side in ASCII. The dealer has one visible
  card and one hidden card during the player turn.
- The player can `hit` or `stand`; totals update automatically after every hit.
- Aces count as 11 when possible and otherwise count as 1.
- A player bust immediately loses and the dealer does not play.
- The dealer card is revealed after the player stands or has blackjack. The
  dealer hits on 16 or less and stands on all 17s, including soft 17.
- Standard wins pay 1:1, pushes return the wager, and natural blackjacks pay
  3:2. A zero balance ends the session.

Out of scope: splitting, double down, insurance, surrender, multi-deck shoes,
persistence, networking, and real-money gambling.

## Assumptions

- This is an educational local program for one terminal user, not a regulated
  gambling product.
- Whole-dollar wagers make player input simple; cents remain necessary to
  represent 3:2 blackjack payouts accurately.
- The dealer does not peek for blackjack before the player acts. This is an
  intentional simplification already agreed for the basic game.

## Tech Stack

- C++17
- GNU C++ compiler (`g++`)
- C++ standard library only; no external dependencies or test framework
- Assertion-based tests in a separate executable

## Commands

Build and run the game:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp blackjack.cpp -o app
./app
```

Build the application, build the tests separately, and run the tests:

```bash
./test_runner.sh
```

## Project Structure

```text
main.cpp                    Terminal prompts, ASCII output, and session loop
blackjack.hpp               Public game-domain types and function declarations
blackjack.cpp               Deck, scoring, dealer policy, and payout logic
tests/blackjack_tests.cpp   Deterministic assertion-based logic tests
test_runner.sh              Application and test build commands
specs/blackjack.md          This accepted behavior specification
README.md                   Concise user build/run guide and rule summary
```

`main.cpp` orchestrates interactive play. `blackjack.cpp` must remain free of
terminal I/O so its rules can be tested with known hands rather than shuffled
gameplay.

## Code Style

- Use C++17 and standard-library types.
- Keep game-domain symbols in the `blackjack` namespace.
- Use `PascalCase` for types/enumerators and `camelCase` for functions and
  variables.
- Prefer small functions with explicit outcomes over hidden state or global
  mutable game state.
- Keep terminal-only helpers in the anonymous namespace in `main.cpp`.

Example:

```cpp
while (blackjack::dealerAction(dealer) == blackjack::DealerAction::Hit) {
  dealer.add(deck.draw());
}
```

## Testing Strategy

`tests/blackjack_tests.cpp` uses standard `assert` calls and tests game logic
without console input or random deck order.

Required automated coverage:

- Ace adjustment, blackjack detection, and bust detection.
- Dealer hit-on-16 and stand-on-soft-17 policy.
- Win, push, loss, and 3:2 blackjack settlement, including the `$10` examples.
- Valid and invalid whole-dollar wagers, including zero balance.
- A 52-card deck containing no duplicate cards after all cards are drawn.

Manual smoke testing should cover ASCII rendering, invalid terminal input,
player bust, dealer play, replay, and end-of-input behavior.

## Boundaries

- **Always:** validate terminal input, keep money in cents, run
  `./test_runner.sh` after behavior changes, and update this spec when rules
  change.
- **Ask first:** add dependencies, change C++ language/toolchain requirements,
  introduce advanced blackjack actions, or alter payout rules.
- **Never:** add real-money payment features, commit generated binaries or
  secrets, or remove a failing test instead of fixing its underlying issue.

## Success Criteria

- [ ] The program starts at `$1,000.00` and rejects invalid or over-balance
      wagers.
- [ ] The player sees ASCII cards and an automatic score after the initial
      deal and every hit.
- [ ] Dealer concealment, reveal timing, and hit/stand policy match the rules
      above.
- [ ] Payouts update the balance accurately and the game ends at zero balance.
- [ ] `./test_runner.sh` compiles both executables and passes all automated
      tests.
- [ ] README and this specification describe the same supported rules.

## Open Questions

None. The basic-game scope and deliberate rule variations have been confirmed.
