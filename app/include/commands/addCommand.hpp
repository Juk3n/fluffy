#pragma once

#include <memory>
#include <iostream>

#include <command.hpp>
#include <gameRepository.hpp>

class AddCommand: public Command {
    private:
        std::weak_ptr<GameRepositoryInterface> gameRepository;
        std::string gameName;
        std::string path;
    public:
        AddCommand(
            std::weak_ptr<GameRepositoryInterface> gameRepository,
            std::string gameName,
            std::string path
        ) {
            this->gameRepository = gameRepository;
            this->gameName = gameName;
            this->path = path;
        }

        void execute() override {
            auto localPath = std::filesystem::path(path);
            std::string globalPath =
                std::filesystem::absolute(localPath).lexically_normal().string();
            std::string temp{};
            for (const auto a : globalPath) {
                if (a != ' ') {
                    temp += a;
                } else {
                    temp += "\' \'";
                }
            }
            if (std::shared_ptr<GameRepositoryInterface> gr = gameRepository.lock()) {
                Game currentGame = gr->getGameByName(gameName);
                if (currentGame.getName().length() != 0) {
                    std::cout << "Game already exist" << std::endl;
                    // output->printMessage("Game already exist");
                }
                else {
                    gr->addGame(gameName, temp);
                }
            }
            
        }
};