#pragma once

class Database
{
private:
    /* data */
public:
    std::string database_initialization_command =
        "CREATE TABLE games("
        "GAME_ID INTEGER PRIMARY KEY AUTOINCREMENT, "
        "GAME_NAME TEXT NOT NULL, "
        "GAME_PATH TEXT NOT NULL);";
    Database(/* args */);
    ~Database();
};
