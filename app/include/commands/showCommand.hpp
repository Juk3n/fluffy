#pragma once

#include <memory>
#include <iostream>

#include <command.hpp>
#include <output.hpp>

class ShowCommand: public Command {
    private:
        std::weak_ptr<GameRepositoryInterface> gameRepository;
        std::weak_ptr<Output> output;
    public:
        ShowCommand(
            std::weak_ptr<GameRepositoryInterface> gameRepository,
            std::weak_ptr<Output> output
        ) {
            this->gameRepository = gameRepository;
            this->output = output;
        }

        void execute() override {
            if (std::shared_ptr<GameRepositoryInterface> repository = gameRepository.lock()) {
                for (auto &game : repository->getGames()) {
                    if (auto out = output.lock()) {
                        out->printMessage(game.getName() + ": " + game.getPath());
                    }
                }
            }
        }
};