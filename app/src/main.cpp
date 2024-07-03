#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/component_options.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"
#include "ftxui/screen/screen.hpp"
#include "ftxui/screen/string.hpp"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <map>
#include <sqlite3.h>
#include <string>
#include <vector>
#include <unistd.h>
#include <limits.h>

#include <game.hpp>
#include <database.hpp>

using namespace ftxui;

std::vector<Game> games;

bool debugRun = false;

auto printMessage(std::string message) -> void {
  if (debugRun) {
    std::cout << message << std::endl;
  }
}

auto addGame(Database& database, std::string name, std::string path) -> void {
  std::string command =
      "INSERT INTO games (GAME_NAME, GAME_PATH) VALUES (";
  command += "'" + name + "', \"" + path + "\");";
  database.execute_sql_command(command);
}

auto removeGame(Database& database, std::string name) -> void{
  std::string command = "DELETE FROM games WHERE GAME_NAME=";
  command += "'" + name + "';";
  database.execute_sql_command(command);
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

auto handleCommand(int argc, char const *argv[], Database& database) -> void{
  std::string command{};
  switch (argc) {
  case 2:
    command = argv[1];
    if (command == "show") {
      for (auto &game : games) {
        std::cout << game.getName() << ": " << game.getPath() << std::endl;
      }
    }
    break;
  case 3:
    command = argv[1];
    if (command == "rm") {
      std::string gameName{argv[2]};
      removeGame(database, gameName);
    } else if (command == "play") {
      std::string gameName{argv[2]};
      runGame(gameName);
    }
  case 4:
    command = argv[1];
    if (command == "add") {
      std::string gameName{argv[2]};
      auto localPath = std::filesystem::path(argv[3]);
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
    break;
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
               center(text(L" Fluffy v0.1 ")),
               appContainer->Render(),
           }) |
           border | size(WIDTH, LESS_THAN, 120);
  });
  menu_screen.Loop(renderer);

}

auto getExecutablePath() -> std::filesystem::path {
  char result[PATH_MAX];
  ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
  return std::filesystem::path(std::string(result, (count > 0) ? count : 0));
}

auto main(int argc, char const *argv[]) -> int {
  auto databasePath{ std::filesystem::path(getExecutablePath().parent_path().string() + "/data.db")};
  Database database{databasePath};
  
  if (!database.database) {
    return -1;
  }
  sqlite3_stmt *stmt;
  int exit = 0;
  exit = sqlite3_prepare_v2(database.database, database.select_all_games_command.c_str(), -1, &stmt, nullptr);

  if (exit) {
    std::cerr << "Error retrieving data" << sqlite3_errmsg(database.database)
              << std::endl;
    sqlite3_close(database.database);
    return -1;
  }

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    int id = sqlite3_column_int(stmt, 0);
    const unsigned char *gameName = sqlite3_column_text(stmt, 1);
    const unsigned char *gamePath = sqlite3_column_text(stmt, 2);

    games.emplace_back(Game{id, reinterpret_cast<const char *>(gameName),
                         reinterpret_cast<const char *>(gamePath)});
  }
  sqlite3_finalize(stmt);

  if (argc > 1) {
    handleCommand(argc, argv, database);
  }
  else {
    runConsoleApp();
  }
  
  return EXIT_SUCCESS;
}