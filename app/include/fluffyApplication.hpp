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
    FluffyApplication(int argc, char const *argv[]);
    
private:
    std::shared_ptr<Output> output{};
    std::string version{ "v0.1.3" };
    std::vector<Game> games;

    auto addGame(Database& database, std::string name, std::string path) -> void;

    auto removeGame(Database& database, std::string name) -> void;

    auto runGame(std::string gameName) -> void;

    auto handleCommand(const std::string& command, const std::vector<std::string>& arguments, Database& database) -> void;

    auto style() -> ButtonOption;

    auto runConsoleApp() -> void;

    auto handleFlags(std::vector<std::string> flags) -> void;

    auto getExecutablePath() -> std::filesystem::path;

    auto parseFlags(int argc, char const *argv[]) -> std::vector<std::string>;

    auto parseCommand(int argc, char const* argv[]);
};