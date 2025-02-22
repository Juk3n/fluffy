#pragma once

#include "gameRepositoryInterface.hpp"
#include "database.hpp"

#include <vector>
#include <memory>

class GameRepository : public GameRepositoryInterface {
private: 
    std::unique_ptr<Database> database = nullptr;
public:
    GameRepository();
    GameRepository(std::unique_ptr<Database> database);
    void addGame(const std::string &name, const std::string &path);
    Game getGameByName(const std::string& name);
    std::vector<Game> getGames();
    void deleteGame(const std::string& name);
};