#include "ftxui/dom/elements.hpp"
#include "ftxui/screen/screen.hpp"
#include "ftxui/screen/string.hpp"

#include <iostream>
#include <string>
#include <cstdlib>
#include <filesystem>
#include <../include/sqlite3.h>
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

void addGame(sqlite3* database, std::string name, std::string path) {
    static int val = 1;
    std::string command = "INSERT INTO GAME (GAME_ID, GAME_NAME, GAME_PATH) VALUES (";
    command += std::to_string(val) + ", '" + name + "', \"" + path + "\");";
    val++;
    execute_sql_command(database, command);
}

void initialize_database(sqlite3* database) {
    execute_sql_command(database, database_initialization_command);
    addGame(database, "Devastating Fog", "");
    addGame(database, "No Pirates Here!", "");
    printMessage("Database Initialized");        
}

int main() {
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
        std::string name{};
        std::string path{};
        name.append(reinterpret_cast<const char*>(gameName));
        path.append(reinterpret_cast<const char*>(gamePath));
        games[name] = path;
    }
    sqlite3_finalize(stmt);

    int choice = 1;
    do {
        system("clear");
        std::vector<Element> localGames = {};
        for (const auto& [name, path] : games) {
            localGames.push_back(hbox({text(" " + name)}) | color(Color::Green));
        }

        auto summary = [&] {
            auto content = vbox(localGames);
            return window(text(L" Fluffy v0.1 "), content);
        };

        auto document =  //
            vbox({
                summary(),
            });

        // Limit the size of the document to 80 char.
        document = document | size(WIDTH, LESS_THAN, 80);

        auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
        Render(screen, document);

        std::cout << screen.ToString() << '\0' << std::endl;
        std::cin >> choice;
        switch (choice)
        {
        case 0:
            break;
        default:
            int i{ 1 };
            std::string pathToRun = "";
            for (const auto& [name, path] : games) {
                if (i == choice) {
                    pathToRun = path;
                    
                }
                i++;
            }
            system(pathToRun.c_str());
            break;
        }
    } while (choice != 0);
  
    sqlite3_close(database);
    return EXIT_SUCCESS;
}