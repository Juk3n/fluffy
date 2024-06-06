#include <iostream>
#include <string>
#include <cstdlib>
#include <filesystem>
#include <../include/sqlite3.h>

std::string gamePath;

std::string database_initialization_command = "CREATE TABLE GAME("
                                                "GAME_ID INT PRIMARY KEY NOT NULL, "
                                                "GAME_NAME TEXT NOT NULL, "
                                                "GAME_PATH TEXT NOT NULL);";

std::string insert_example_game_command = "INSERT INTO GAME (GAME_ID, GAME_NAME, GAME_PATH) VALUES (1, 'Devastating Fog', \"\");";

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
    exit = sqlite3_exec(database, database_initialization_command.c_str(), NULL, 0, &messageError);    
    
    if (exit != SQLITE_OK) {
        printMessage("Error with command: " + command);
        sqlite3_free(messageError);
    }
}

void initialize_database(sqlite3* database) {
    execute_sql_command(database, database_initialization_command.c_str());
    execute_sql_command(database, insert_example_game_command.c_str());
    printMessage("Database Initialized");        
}

int main() {
    bool newCreation = not std::filesystem::exists("example_database");

    char* messageError;
    sqlite3* database;
    sqlite3_stmt* stmt;
    int exit = 0;
    exit = sqlite3_open("example_database", &database);

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
        const unsigned char* game_name = sqlite3_column_text(stmt, 1);
        const unsigned char* path = sqlite3_column_text(stmt, 2);

        std::cout << game_name << ": " << path << std::endl;
        gamePath.append(reinterpret_cast<const char*>(path));
    }
    sqlite3_finalize(stmt);

    sqlite3_close(database);
    system(gamePath.c_str());
    return 0;
}