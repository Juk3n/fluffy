#pragma once

#include "game.hpp"

class GameRepositoryInterface {
public:
    virtual void addGame(const Game& game) = 0;
    virtual Game getGameByName(const std::string& name) = 0;
    virtual void deleteGame(const std::string& name) = 0;
};