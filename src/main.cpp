#include "ftxui/dom/elements.hpp"
#include "ftxui/screen/screen.hpp"
#include "ftxui/screen/string.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/component_options.hpp"
#include "ftxui/component/screen_interactive.hpp"

#include <iostream>
#include <string>
#include <cstdlib>
#include <filesystem>
#include <sqlite3.h>
#include <map>


using namespace ftxui;

std::map<std::string, std::string> games;

std::string database_initialization_command = "CREATE TABLE GAME("
                                                "GAME_ID INT PRIMARY KEY NOT NULL, "
                                                "GAME_NAME TEXT NOT NULL, "
                                                "GAME_PATH TEXT NOT NULL);";

const char* select_all_games_command = "SELECT * FROM GAME;";

bool debugRun = true;

void printMessage(std::string message) {
    if (debugRun) {
        std::cout << message << std::endl;
    }
}

void execute_sql_command(sqlite3* database, std::string command) {
    int exit = 0;
    char* messageError;
    exit = sqlite3_exec(database, command.c_str(), NULL, 0, &messageError);    
    
    if (exit != SQLITE_OK) {
        printMessage("Error with command: " + command + " " + messageError);
        sqlite3_free(messageError);
    }
}

void addGame(sqlite3* database, int id,  std::string name, std::string path) {
    std::string command = "INSERT INTO GAME (GAME_ID, GAME_NAME, GAME_PATH) VALUES (";
    command += std::to_string(id) + ", '" + name + "', \"" + path + "\");";
    execute_sql_command(database, command);
}

void initialize_database(sqlite3* database) {
    execute_sql_command(database, database_initialization_command);
    addGame(database, 1, "Devastating Fog", "");
    addGame(database, 2, "No Pirates Here!", "");
    printMessage("Database Initialized");        
}

int main(int argc, char const *argv[]) {
    bool newCreation = not std::filesystem::exists("data.db");

    char* messageError;
    sqlite3* database;
    sqlite3_stmt* stmt;
    int exit = 0;
    exit = sqlite3_open("data.db", &database);

    if (exit) {
        std::cerr << "Error opening database" << sqlite3_errmsg(database) << std::endl;
        return -1;
    } 
    
    std::cout << "Opened database successfully!" << std::endl;

    if (newCreation) {
        initialize_database(database);
    }

    exit = sqlite3_prepare_v2(database, select_all_games_command, -1, &stmt, 0);

    if (exit) {
        std::cerr << "Error retrieving data" << sqlite3_errmsg(database) << std::endl;
        sqlite3_close(database);
        return -1;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char* gameName = sqlite3_column_text(stmt, 1);
        const unsigned char* gamePath = sqlite3_column_text(stmt, 2);

        std::cout << gameName << ": " << gamePath << std::endl;
        std::string name{reinterpret_cast<const char*>(gameName)};
        std::string path{reinterpret_cast<const char*>(gamePath)};
        games[name] = path;
    }
    sqlite3_finalize(stmt);

    if (argc == 2) {
        return 0;
    }

    if (argc == 4) {
        std::string command{ argv[1] };
        std::string gameName{ argv[2] };
        auto localPath = std::filesystem::path(argv[3]);
        std::string globalPath = std::filesystem::absolute(localPath).lexically_normal().string();
        
        std::string temp{};
        for (const auto a : globalPath) {
            if (a != ' ') {
                temp += a;
            }
            else {
                temp += "\' \'";
            }
        }

        addGame(database, games.size() + 1, gameName, temp);
        return 0;
    }

    int exit_id = games.size();
    int choice = 0;
    do {
        system("clear");
        auto document = vbox({
            center(text(L" Fluffy v0.1 ")),
        });
        
        document = document | size(WIDTH, LESS_THAN, 120);

        auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
        Render(screen, document);

        std::cout << screen.ToString() << '\0' << std::endl;
        
        
        auto menu_screen = ScreenInteractive::TerminalOutput();

        std::vector<std::string> localGames = {};
        for (const auto& [name, path] : games) {
            localGames.push_back(name);
        }
        localGames.push_back("Exit");

        MenuOption option;
        option.on_enter = menu_screen.ExitLoopClosure();
        auto menu = Menu(&localGames, &choice, option);
        menu_screen.Loop(menu);
        
        if (choice != exit_id) {
            int i{ 0 };
            std::string pathToRun = "";
            for (const auto& [name, path] : games) {
                
                if (i == choice) {
                    pathToRun = path;
                    
                }
                i++;
            }
            system(pathToRun.c_str());
        }
    } while (choice != exit_id);
    sqlite3_close(database);
    return EXIT_SUCCESS;
}