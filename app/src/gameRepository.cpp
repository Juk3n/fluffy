#include "gameRepository.hpp"

#include <filesystem>

GameRepository::GameRepository()
{
    this->database = nullptr;
}

GameRepository::GameRepository(std::unique_ptr<Database> database)
{
    this->database = std::move(database);
}

void GameRepository::addGame(const std::string &name, const std::string &path)
{
    std::string command =
        "INSERT INTO games (GAME_NAME, GAME_PATH) VALUES (";
    command += "'" + name + "', \"" + path + "\");";
    this->database->executeSqlCommand(command);
}

Game GameRepository::getGameByName(const std::string &name)
{
    auto games = this->database->getGames();
    for (auto &game : games) {
        if (name == game.getName()) {
            return game;
        }
    }
    return Game(0, "", "");
}

std::vector<Game> GameRepository::getGames()
{
    return this->database->getGames();
}

void GameRepository::deleteGame(const std::string &name)
{
    std::string command = "DELETE FROM games WHERE GAME_NAME=";
    command += "'" + name + "';";
    this->database->executeSqlCommand(command);
}