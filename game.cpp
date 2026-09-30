#include "game.hpp"

#include <cctype>
#include <iostream>
#include <utility>

Game::Game(std::istream& input, std::ostream& output)
    : input_(input), output_(output) {}

void Game::run() {
    showIntro();

    while (running_) {
        showLocation();

        std::string command;
        if (!std::getline(input_, command)) {
            output_ << "Input ended. Thanks for playing Codequest.\n";
            running_ = false;
            break;
        }

        handleCommand(normalize(std::move(command)));
    }
}

void Game::showIntro() {
    output_ << "Welcome to Codequest. Explore, collect the clues, and open the "
               "Observatory vault.\n";
}

void Game::showLocation() const {
    switch (currentLocation_) {
        case Location::Camp:
            output_ << "\nCamp\n"
                       "1. Visit the Archive\n"
                       "2. Visit the Workshop\n"
                       "3. Visit the Observatory\n";
            break;
        case Location::Archive:
            output_ << "\nArchive\n"
                       "1. Search the shelves\n"
                       "2. Return to Camp\n";
            break;
        case Location::Workshop:
            output_ << "\nWorkshop\n"
                       "1. Search the workbench\n"
                       "2. Return to Camp\n";
            break;
        case Location::Observatory:
            output_ << "\nObservatory\n";
            if (hasStarChart_ && hasDecoderWheel_) {
                output_ << "1. Attempt the riddle\n"
                           "2. Return to Camp\n";
            } else {
                if (!hasStarChart_ && !hasDecoderWheel_) {
                    output_ << "You still need the Star Chart and Decoder Wheel.\n";
                } else if (!hasStarChart_) {
                    output_ << "You still need the Star Chart.\n";
                } else {
                    output_ << "You still need the Decoder Wheel.\n";
                }
                output_ << "1. Return to Camp\n";
            }
            break;
    }

    output_ << "Global commands: help, inventory, quit.\n> ";
}

void Game::handleCommand(const std::string& command) {
    if (handleGlobalCommand(command)) {
        return;
    }

    switch (currentLocation_) {
        case Location::Camp:
            if (command == "1") {
                currentLocation_ = Location::Archive;
            } else if (command == "2") {
                currentLocation_ = Location::Workshop;
            } else if (command == "3") {
                currentLocation_ = Location::Observatory;
            } else {
                showInvalidCommand();
            }
            break;
        case Location::Archive:
            visitArchive(command);
            break;
        case Location::Workshop:
            visitWorkshop(command);
            break;
        case Location::Observatory:
            visitObservatory(command);
            break;
    }
}

bool Game::handleGlobalCommand(const std::string& command) {
    if (command == "help") {
        showHelp();
        return true;
    }
    if (command == "inventory") {
        showInventory();
        return true;
    }
    if (command == "quit") {
        quit();
        return true;
    }
    return false;
}

void Game::visitArchive(const std::string& command) {
    if (command == "1") {
        if (hasStarChart_) {
            output_ << "You already collected the Star Chart.\n";
        } else {
            hasStarChart_ = true;
            output_ << "You collect the Star Chart. The chart shows a constellation "
                       "named after a famous hunter. Its name begins with O.\n";
        }
    } else if (command == "2") {
        currentLocation_ = Location::Camp;
    } else {
        showInvalidCommand();
    }
}

void Game::visitWorkshop(const std::string& command) {
    if (command == "1") {
        if (hasDecoderWheel_) {
            output_ << "You already collected the Decoder Wheel.\n";
        } else {
            hasDecoderWheel_ = true;
            output_ << "You collect the Decoder Wheel. The decoder reveals the "
                       "Hunter's constellation has five letters and ends with N.\n";
        }
    } else if (command == "2") {
        currentLocation_ = Location::Camp;
    } else {
        showInvalidCommand();
    }
}

void Game::visitObservatory(const std::string& command) {
    if (!hasStarChart_ || !hasDecoderWheel_) {
        if (command == "1") {
            currentLocation_ = Location::Camp;
        } else {
            showInvalidCommand();
        }
        return;
    }

    if (command == "1") {
        tryPuzzle();
    } else if (command == "2") {
        currentLocation_ = Location::Camp;
    } else {
        showInvalidCommand();
    }
}

void Game::showHelp() const {
    output_ << "Global commands: help, inventory, quit. Use the listed number for "
               "a location action.\n";
}

void Game::showInventory() const {
    if (!hasStarChart_ && !hasDecoderWheel_) {
        output_ << "Inventory: empty.\n";
        return;
    }

    output_ << "Inventory:";
    if (hasStarChart_) {
        output_ << " Star Chart";
    }
    if (hasDecoderWheel_) {
        output_ << " Decoder Wheel";
    }
    output_ << '\n';
}

void Game::tryPuzzle() {
    output_ << "What is the name of the Hunter constellation? It starts with O, has "
               "five letters, and ends with N.\n> ";

    std::string answer;
    if (!std::getline(input_, answer)) {
        output_ << "Input ended. Thanks for playing Codequest.\n";
        running_ = false;
        return;
    }

    answer = normalize(std::move(answer));
    if (handleGlobalCommand(answer)) {
        return;
    }
    if (answer == "orion") {
        won_ = true;
        running_ = false;
        output_ << "Codequest complete! The Observatory vault opens, and your quest "
                   "is complete.\n";
        return;
    }

    output_ << "That is not the answer.\n";
}

void Game::quit() {
    output_ << "Thanks for playing Codequest.\n";
    running_ = false;
}

void Game::showInvalidCommand() const {
    output_ << "Please enter a listed command.\n";
}

std::string Game::normalize(std::string text) {
    const auto first = text.find_first_not_of(" \t\n\r\f\v");
    if (first == std::string::npos) {
        return "";
    }

    const auto last = text.find_last_not_of(" \t\n\r\f\v");
    text = text.substr(first, last - first + 1);
    for (char& character : text) {
        character = static_cast<char>(std::tolower(static_cast<unsigned char>(character)));
    }
    return text;
}
