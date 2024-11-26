#pragma once


#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/screen/string.hpp>

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <map>
#include <sqlite3.h>
#include <string>
#include <vector>
#include <unistd.h>
#include <limits.h>
#include <memory>

#include <game.hpp>
#include <database.hpp>
#include <output.hpp>

using namespace ftxui;

class FluffyApplication {
public:
    std::shared_ptr<Output> output{};
    std::string version{ "v0.1.2" };
    std::vector<Game> games;

    FluffyApplication(int argc, char const *argv[]);
    
    auto addGame(Database& database, std::string name, std::string path) -> void {
    std::string command =
        "INSERT INTO games (GAME_NAME, GAME_PATH) VALUES (";
    command += "'" + name + "', \"" + path + "\");";
    database.executeSqlCommand(command);
    }

    auto removeGame(Database& database, std::string name) -> void{
    std::string command = "DELETE FROM games WHERE GAME_NAME=";
    command += "'" + name + "';";
    database.executeSqlCommand(command);
    }

    auto runGame(std::string gameName) -> void {
    std::string pathToRun = "";
    for (auto &game : games) {
        if (gameName == game.getName()) {
        pathToRun = game.getPath();
        }
    }
    system(pathToRun.c_str());
    }

    auto handleCommand(const std::string& command, const std::vector<std::string>& arguments, Database& database) -> void{
    if (command == "show") {
        for (auto &game : games) {
        std::cout << game.getName() << ": " << game.getPath() << std::endl;
        }
    }
    else if (command == "rm") {
        std::string gameName{arguments[0]};
        removeGame(database, gameName);
    }
    else if (command == "add") {
        std::string gameName{arguments[0]};
        auto localPath = std::filesystem::path(arguments[1]);
        std::string globalPath =
            std::filesystem::absolute(localPath).lexically_normal().string();
        std::string temp{};
        for (const auto a : globalPath) {
        if (a != ' ') {
            temp += a;
        } else {
            temp += "\' \'";
        }
        }

        addGame(database, gameName, temp);
    }
    else if (command == "play") {
        std::string gameName{arguments[0]};
        runGame(gameName);
    }
    else if (command == "--version") {
        output->printMessage("fluffy " + version);
    }
    else if (command == "--help") {
        output->printMessage("fluffy commands:\n \
    play <game_name>              Runs a game\n \
    add <game_name> <game_path>   Add a game library\n \
    show                          Show all games added to library\n \
    rm <game_name>                Remove game from a library");
    }
    else {
        output->printMessage("fluffy: '" + command + "' is not a fluffy command. See 'fluffy --help'");
    }
    }

    auto style() -> ButtonOption {
    auto option = ButtonOption::Border();
    option.transform = [](const EntryState &s) {
        auto element = text(s.label);
        if (s.focused) {
        element |= bold;
        }
        return element | size(HEIGHT, LESS_THAN, 10);
    };
    return option;
    }

    auto runConsoleApp() -> void {
    system("clear");

    std::vector<std::string> localGames = {};
    for (auto &game : games) {
        localGames.emplace_back(game.getName());
    }
    localGames.emplace_back("Exit");

    auto gamesContainer{Container::Vertical({})};
    for (auto &game : games) {
        gamesContainer->Add(Button(
            game.getName(), [&] { runGame(game.getName()); }, style()));
    }

    auto menu_screen = ScreenInteractive::TerminalOutput();

    auto quitButton = Button("Exit", menu_screen.ExitLoopClosure(), style());
    gamesContainer->Add(quitButton);

    int menuOptionSelected{0};
    auto appContainer =
        Container::Tab({gamesContainer}, &menuOptionSelected);

    auto container = Container::Horizontal({
        appContainer,
    });
    auto renderer = Renderer(container, [&] {
        return vbox({
                center(text("Fluffy " + version)),
                appContainer->Render(),
            }) |
            border | size(WIDTH, LESS_THAN, 120);
    });
    menu_screen.Loop(renderer);

    }

    auto handleFlags(std::vector<std::string> flags) -> void {
    for (const auto& flag : flags) {
        if (flag == "-d") {
        output->turnOnDebug();
        }
        else {
        output->printMessage("unkown option: '" + flag + "'");
        }
    }
    }

    auto getExecutablePath() -> std::filesystem::path {
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    return std::filesystem::path(std::string(result, (count > 0) ? count : 0));
    }

    auto parseFlags(int argc, char const *argv[]) -> std::vector<std::string> {
    std::vector<std::string> flags;
    for (int i = 1; i < argc; i ++) {
        std::string appArgument = argv[i];
        if (appArgument.size() < 2) continue;
        if (appArgument[0] == '-' and appArgument[1] != '-') flags.emplace_back(appArgument);
    }
    return flags;
    }

    auto parseCommand(int argc, char const* argv[]) {
    std::string command{};
    std::vector<std::string> arguments{};
    for (int i = 1; i < argc; i ++) {
        std::string appArgument = argv[i];
        if (appArgument.size() < 2) continue;
        if ((appArgument[0] == '-' and appArgument[1] == '-') or appArgument[0] != '-') {
        if (command == "") command = appArgument;
        else arguments.emplace_back(appArgument);
        }
    }
    struct Command { std::string command; std::vector<std::string> arguments; };
    return Command{command, arguments};
    }        
};