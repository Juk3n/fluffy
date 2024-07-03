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
    std::string select_all_games_command = "SELECT * FROM games;";

    void execute_sql_command(sqlite3 *database, std::string command) {
        int exit = 0;
        char *messageError;
        exit = sqlite3_exec(database, command.c_str(), nullptr, nullptr, &messageError);

        if (exit != SQLITE_OK) {
            //printMessage("Error with command: " + command + " " + messageError);
            sqlite3_free(messageError);
        }
    }

    void initialize_database(sqlite3 *database) {
        execute_sql_command(database, database_initialization_command);
        //printMessage("Database Initialized");
    }
};
