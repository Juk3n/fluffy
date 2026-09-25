#pragma once

#include <memory>
#include <iostream>

#include <command.hpp>


class ShowCommand: public Command {
    private:
        std::weak_ptr<GameRepositoryInterface> gameRepository;
    public:
        ShowCommand(std::weak_ptr<GameRepositoryInterface> gameRepository) {
            this->gameRepository = gameRepository;
        }

        void execute() override {
            if (std::shared_ptr<GameRepositoryInterface> repository = gameRepository.lock()) {
                for (auto &game : repository->getGames()) {
                    std::cout << game.getName() << ": " << game.getPath() << std::endl;
                }
            }
        }
};