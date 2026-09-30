#ifndef CODEQUEST_GAME_HPP
#define CODEQUEST_GAME_HPP

#include <iosfwd>
#include <string>

enum class Location { Camp, Archive, Workshop, Observatory };
enum class Item { StarChart, DecoderWheel };

class Game {
public:
    Game(std::istream& input, std::ostream& output);

    void run();

private:
    std::istream& input_;
    std::ostream& output_;
    Location currentLocation_ = Location::Camp;
    bool hasStarChart_ = false;
    bool hasDecoderWheel_ = false;
    bool running_ = true;
    bool won_ = false;

    void showIntro();
    void showLocation() const;
    void handleCommand(const std::string& command);
    bool handleGlobalCommand(const std::string& command);
    void visitArchive(const std::string& command);
    void visitWorkshop(const std::string& command);
    void visitObservatory(const std::string& command);
    void showHelp() const;
    void showInventory() const;
    void tryPuzzle();
    void quit();
    void showInvalidCommand() const;
    static std::string normalize(std::string text);
};

#endif
