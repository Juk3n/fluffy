#pragma once

#include "game.hpp"

#include <vector>

class GameRepositoryInterface {
public:
    virtual void addGame(const std::string &name, const std::string &path) = 0;
    
    virtual Game getGameByName(const std::string& name) = 0;
    
    virtual std::vector<Game> getGames() = 0;
    
    virtual void deleteGame(const std::string& name) = 0;
};