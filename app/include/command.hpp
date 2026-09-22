#pragma once

#include <memory>

#include "gameRepository.hpp";

class Command {
    public:
        virtual void execute() = 0;
        virtual ~Command() {}
};

class ShowCommand: public Command {
    private:
        std::weak_ptr<GameRepositoryInterface> gameRepository;
    public:
        void execute() override {
            if (std::shared_ptr<GameRepositoryInterface> repository = gameRepository.lock()) {
                for (auto &game : repository->getGames()) {
                    std::cout << game.getName() << ": " << game.getPath() << std::endl;
                }
            }
        }
};