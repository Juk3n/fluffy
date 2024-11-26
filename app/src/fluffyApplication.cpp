#include "fluffyApplication.hpp"

#include <iostream>

FluffyApplication::FluffyApplication(int argc, char const *argv[]) {
    output = std::make_shared<Output>();
    auto flags = parseFlags(argc, argv);
    handleFlags(flags);

    auto databasePath{ std::filesystem::path(getExecutablePath().parent_path().string() + "/data.db")};
    Database database{databasePath, output};
    games = database.getGames();

    if (argc - flags.size() > 1) {
      auto [command, arguments] = parseCommand(argc, argv);
      if (command != "") handleCommand(command, arguments, database);
    }
    else {
      runConsoleApp();
    }    
}