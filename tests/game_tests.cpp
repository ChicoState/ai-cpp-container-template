#include "game.hpp"

#include <iostream>
#include <sstream>
#include <string>

namespace {

int failures = 0;

void expect(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        ++failures;
    }
}

std::string runGame(const std::string& commands) {
    std::istringstream input(commands);
    std::ostringstream output;
    Game game(input, output);
    game.run();
    return output.str();
}

std::size_t countOccurrences(const std::string& text, const std::string& value) {
    std::size_t count = 0;
    std::size_t position = 0;
    while ((position = text.find(value, position)) != std::string::npos) {
        ++count;
        position += value.size();
    }
    return count;
}

void testCompleteWinningRoute() {
    const std::string output = runGame(
        "1\n1\n2\n2\n1\n2\n3\n1\n  oRiOn  \n");

    expect(output.find("You collect the Star Chart.") != std::string::npos,
           "the Archive awards the Star Chart");
    expect(output.find("You collect the Decoder Wheel.") != std::string::npos,
           "the Workshop awards the Decoder Wheel");
    expect(output.find("The chart shows a constellation named after a famous hunter. "
                       "Its name begins with O.") != std::string::npos,
           "the Archive displays the updated constellation clue");
    expect(output.find("The decoder reveals the Hunter's constellation has five "
                       "letters and ends with N.") != std::string::npos,
           "the Workshop displays the updated constellation clue");
    expect(output.find("What is the name of the Hunter constellation? It starts with "
                       "O, has five letters, and ends with N.") != std::string::npos,
           "the Observatory displays the updated riddle");
    expect(output.find("Codequest complete!") != std::string::npos,
           "a normalized ORION answer wins the game");
}

void testItemsCanBeCollectedInEitherOrder() {
    const std::string output = runGame("2\n1\n2\n1\n1\nquit\n");

    const std::size_t wheel = output.find("You collect the Decoder Wheel.");
    const std::size_t chart = output.find("You collect the Star Chart.");
    expect(wheel != std::string::npos && chart != std::string::npos && wheel < chart,
           "the Workshop can be completed before the Archive");
}

void testCollectionIsIdempotent() {
    const std::string output = runGame("1\n1\n1\nquit\n");

    expect(countOccurrences(output, "You collect the Star Chart.") == 1,
           "the Star Chart is awarded only once");
    expect(output.find("already collected the Star Chart") != std::string::npos,
           "a repeat search explains that the item is already collected");
}

void testObservatoryRequiresBothItems() {
    const std::string withoutItems = runGame("3\nquit\n");
    const std::string withOneItem = runGame("1\n1\n2\n3\nquit\n");

    expect(withoutItems.find("You still need the Star Chart and Decoder Wheel.") !=
               std::string::npos,
           "the Observatory identifies both missing items");
    expect(withOneItem.find("You still need the Decoder Wheel.") != std::string::npos,
           "the Observatory identifies the remaining item");
}

void testWrongAnswerDoesNotWin() {
    const std::string output = runGame(
        "1\n1\n2\n2\n1\n2\n3\n1\npegasus\nquit\n");

    expect(output.find("That is not the answer.") != std::string::npos,
           "a wrong answer receives feedback");
    expect(output.find("Codequest complete!") == std::string::npos,
           "a wrong answer does not win the game");
    expect(output.find("Thanks for playing Codequest.") != std::string::npos,
           "the player can continue and quit after a wrong answer");
}

void testHelpInventoryAndInvalidInput() {
    const std::string output = runGame("help\ninventory\n\nunknown\nquit\n");

    expect(output.find("Global commands: help, inventory, quit.") !=
               std::string::npos,
           "help lists global commands");
    expect(output.find("Inventory: empty.") != std::string::npos,
           "inventory reports an empty inventory");
    expect(countOccurrences(output, "Please enter a listed command.") == 2,
           "blank and unknown input receive validation feedback");
}

void testQuitAndEofExitCleanly() {
    const std::string quitOutput = runGame("quit\n");
    const std::string eofOutput = runGame("");

    expect(quitOutput.find("Thanks for playing Codequest.") != std::string::npos,
           "quit exits with a completion message");
    expect(eofOutput.find("Input ended. Thanks for playing Codequest.") !=
               std::string::npos,
           "EOF exits without a prompt loop");
}

}  // namespace

int main() {
    testCompleteWinningRoute();
    testItemsCanBeCollectedInEitherOrder();
    testCollectionIsIdempotent();
    testObservatoryRequiresBothItems();
    testWrongAnswerDoesNotWin();
    testHelpInventoryAndInvalidInput();
    testQuitAndEofExitCleanly();

    if (failures == 0) {
        std::cout << "All Codequest tests passed.\n";
        return 0;
    }

    std::cerr << failures << " Codequest test(s) failed.\n";
    return 1;
}
