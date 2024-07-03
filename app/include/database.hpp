#pragma once

#include <sqlite3.h>
#include <string>

#include <game.hpp>

class Database
{
private:
    sqlite3 *database;

    std::string database_initialization_command =
        "CREATE TABLE games("
        "GAME_ID INTEGER PRIMARY KEY AUTOINCREMENT, "
        "GAME_NAME TEXT NOT NULL, "
        "GAME_PATH TEXT NOT NULL);";
    std::string select_all_games_command = "SELECT * FROM games;";

    auto initializeDatabase() -> void {
        executeSqlCommand(database_initialization_command);
        //printMessage("Database Initialized");
    }

public:
    bool successfullyCreated{ true };

    Database(std::filesystem::path databasePath) {
        loadDatabase(databasePath);
    }

    ~Database() {
        sqlite3_close(database);
    }

    auto executeSqlCommand(std::string command) -> void {
        int exit = 0;
        char *messageError;
        exit = sqlite3_exec(database, command.c_str(), nullptr, nullptr, &messageError);

        if (exit != SQLITE_OK) {
            //printMessage("Error with command: " + command + " " + messageError);
            sqlite3_free(messageError);
            successfullyCreated = false;
        }
    }



    auto loadDatabase(std::filesystem::path databasePath) -> void {
        bool newCreation = not std::filesystem::exists(databasePath.string());
        
        int exit = 0;
        exit = sqlite3_open(databasePath.c_str(), &database);

        if (exit) {
            std::cerr << "Error opening database" << sqlite3_errmsg(database)
                    << std::endl;
            successfullyCreated = false;
        }

        //printMessage("Opened database successfully!");

        if (newCreation) {
            initializeDatabase();
        }
    }

    auto getGames() -> std::vector<Game> {
        sqlite3_stmt *stmt;
        int exit = 0;
        exit = sqlite3_prepare_v2(database, select_all_games_command.c_str(), -1, &stmt, nullptr);

        if (exit) {
            std::cerr << "Error retrieving data" << sqlite3_errmsg(database)
                    << std::endl;
            sqlite3_close(database);
            return {};
        }

        std::vector<Game> games;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            const unsigned char *gameName = sqlite3_column_text(stmt, 1);
            const unsigned char *gamePath = sqlite3_column_text(stmt, 2);

            games.emplace_back(Game{id, reinterpret_cast<const char *>(gameName),
                                reinterpret_cast<const char *>(gamePath)});
        }
        sqlite3_finalize(stmt);
        return games;
    }
};
