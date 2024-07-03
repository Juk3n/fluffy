#pragma once

#include <sqlite3.h>
#include <string>

class Database
{
private:
    std::string database_initialization_command =
        "CREATE TABLE games("
        "GAME_ID INTEGER PRIMARY KEY AUTOINCREMENT, "
        "GAME_NAME TEXT NOT NULL, "
        "GAME_PATH TEXT NOT NULL);";
public:
    sqlite3 *database;
    std::string select_all_games_command = "SELECT * FROM games;";

    Database(std::filesystem::path databasePath) {
        database = load_database(databasePath);
    }
    Database() = default;

    ~Database() {
        sqlite3_close(database);
    }

    auto execute_sql_command(std::string command) -> void {
        int exit = 0;
        char *messageError;
        exit = sqlite3_exec(database, command.c_str(), nullptr, nullptr, &messageError);

        if (exit != SQLITE_OK) {
            //printMessage("Error with command: " + command + " " + messageError);
            sqlite3_free(messageError);
        }
    }

    auto initialize_database() -> void {
        execute_sql_command(database_initialization_command);
        //printMessage("Database Initialized");
    }

    auto load_database(std::filesystem::path databasePath) -> sqlite3* {
        bool newCreation = not std::filesystem::exists(databasePath.string());
        
        int exit = 0;
        exit = sqlite3_open(databasePath.c_str(), &database);

        if (exit) {
            std::cerr << "Error opening database" << sqlite3_errmsg(database)
                    << std::endl;
            return nullptr;
        }

        //printMessage("Opened database successfully!");

        if (newCreation) {
            initialize_database();
        }
        return database;
    }
};
