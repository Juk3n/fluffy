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

#include <game.hpp>

using namespace ftxui;

std::vector<Game> games;

std::string database_initialization_command =
    "CREATE TABLE games("
    "GAME_ID INTEGER PRIMARY KEY AUTOINCREMENT, "
    "GAME_NAME TEXT NOT NULL, "
    "GAME_PATH TEXT NOT NULL);";

std::string select_all_games_command = "SELECT * FROM games;";

bool debugRun = true;

void printMessage(std::string message) {
  if (debugRun) {
    std::cout << message << std::endl;
  }
}

void execute_sql_command(sqlite3 *database, std::string command) {
  int exit = 0;
  char *messageError;
  exit = sqlite3_exec(database, command.c_str(), NULL, 0, &messageError);

  if (exit != SQLITE_OK) {
    printMessage("Error with command: " + command + " " + messageError);
    sqlite3_free(messageError);
  }
}

void addGame(sqlite3 *database, std::string name, std::string path) {
  std::string command =
      "INSERT INTO games (GAME_NAME, GAME_PATH) VALUES (";
  command += "'" + name + "', \"" + path + "\");";
  execute_sql_command(database, command);
}

void removeGame(sqlite3 *database, std::string name) {
  std::string command = "DELETE FROM games WHERE GAME_NAME=";
  command += "'" + name + "';";
  execute_sql_command(database, command);
}

void initialize_database(sqlite3 *database) {
  execute_sql_command(database, database_initialization_command);
  printMessage("Database Initialized");
}

void runGame(sqlite3 *database, std::string gameName) {
  std::string pathToRun = "";
  for (auto &game : games) {
    if (gameName == game.getName()) {
      pathToRun = game.getPath();
    }
  }
  system(pathToRun.c_str());
}

void handleCommand(int argc, char const *argv[], sqlite3 *database) {
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
      runGame(database, gameName);
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

auto Style() -> ButtonOption {
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

auto main(int argc, char const *argv[]) -> int {
  bool newCreation = not std::filesystem::exists("data.db");
  char *messageError;
  sqlite3 *database;
  sqlite3_stmt *stmt;
  int exit = 0;
  exit = sqlite3_open("data.db", &database);

  if (exit) {
    std::cerr << "Error opening database" << sqlite3_errmsg(database)
              << std::endl;
    return -1;
  }

  printMessage("Opened database successfully!");

  if (newCreation) {
    initialize_database(database);
  }

  exit = sqlite3_prepare_v2(database, select_all_games_command.c_str(), -1, &stmt, 0);

  if (exit) {
    std::cerr << "Error retrieving data" << sqlite3_errmsg(database)
              << std::endl;
    sqlite3_close(database);
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
    return 0;
  }

  int exit_id = games.size();
  int choice = 0;
  // do {
  system("clear");

  std::vector<std::string> menuOptions{
      "Games (old)",
      "Games",
  };
  int menuOptionSelected{0};
  auto menuOptionToggle = Toggle(&menuOptions, &menuOptionSelected);

  std::vector<std::string> localGames = {};
  for (auto &game : games) {
    localGames.emplace_back(game.getName());
  }
  localGames.emplace_back("Exit");

  auto gamesContainer{Container::Vertical({})};
  for (auto &game : games) {
    gamesContainer->Add(Button(
        game.getName(), [&] { runGame(database, game.getName()); }, Style()));
  }

  auto menu_screen = ScreenInteractive::TerminalOutput();
  gamesContainer->Add(Button(
      "Exit", [&] { menu_screen.ExitLoopClosure(); }, Style()));
  MenuOption option;
  // option.on_enter = menu_screen.ExitLoopClosure();
  auto menu = Menu(&localGames, &choice, option);

  auto appContainer =
      Container::Tab({menu, gamesContainer}, &menuOptionSelected);

  auto container = Container::Horizontal({
      menuOptionToggle,
      appContainer,
  });

  auto renderer = Renderer(container, [&] {
    return vbox({
               center(text(L" Fluffy v0.1 ")),
               menuOptionToggle->Render(),
               appContainer->Render(),
           }) |
           border | size(WIDTH, LESS_THAN, 120);
  });

  menu_screen.Loop(renderer);

  // if (choice != exit_id) {
  //   int i{0};
  //   std::string gameToRun = "";
  //   for (auto &game : games) {

  //     if (i == choice) {
  //       gameToRun = game.getName();
  //     }
  //     i++;
  //   }
  //   runGame(database, gameToRun);
  // }
  // } while (choice != exit_id);
  sqlite3_close(database);
  return EXIT_SUCCESS;
}