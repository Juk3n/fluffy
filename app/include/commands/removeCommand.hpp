#pragma once

#include <memory>
#include <iostream>

#include <command.hpp>


class RemoveCommand: public Command {
    private:
        std::weak_ptr<GameRepositoryInterface> gameRepository;
        std::string gameName;
    public:
        RemoveCommand(
            std::weak_ptr<GameRepositoryInterface> gameRepository,
            const std::string &gameName
        ) {
            this->gameRepository = gameRepository;
            this->gameName = gameName;
        }

        void execute() override {
            if(std::shared_ptr<GameRepositoryInterface> gr = gameRepository.lock()) {
                gr->deleteGame(gameName);
            }
        }
};