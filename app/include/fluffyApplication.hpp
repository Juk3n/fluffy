#pragma once

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
#include <filesystem>

#include <game.hpp>
#include <database.hpp>
#include <output.hpp>
#include <gameRepository.hpp>
#include <command.hpp>
#include <commands/showCommand.hpp>
#include <commands/removeCommand.hpp>
#include <commands/playCommand.hpp>

class FluffyApplication {
public:
    FluffyApplication(int argc, char const *argv[]);
    
private:
    std::shared_ptr<GameRepositoryInterface> gameRepository = nullptr;
    std::shared_ptr<Output> output{};
    std::string version{ "v0.1.3" };
    std::string helpMessage{
        "usage: fluffy [--flag | <command> <args>]\n\
fluffy flags:\n\
    --help\n\
    --version\n\
fluffy commands:\n\
    play <game_name>              Runs a game\n\
    add <game_name> <game_path>   Add a game library\n\
    show                          Show all games added to library\n\
    rm <game_name>                Remove game from a library"};
    
    auto addGame(std::string name, std::string path) -> void;

    auto removeGame(Database& database, std::string name) -> void;

    auto runGame(std::string gameName) -> void;

    auto handleCommand(const std::string& command, const std::vector<std::string>& arguments) -> void;

    auto handleFlags(std::vector<std::string> flags) -> void;

    auto getExecutablePath() -> std::filesystem::path;

    auto parseFlags(int argc, char const *argv[]) -> std::vector<std::string>;

    auto parseCommand(int argc, char const* argv[]);
};