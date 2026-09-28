#pragma once

#include <memory>
#include <iostream>

#include <command.hpp>
#include <gameRepository.hpp>
#include <output.hpp>

class AddCommand: public Command {
    private:
        std::weak_ptr<GameRepositoryInterface> gameRepository;
        std::weak_ptr<Output> output;
        std::string gameName;
        std::string path;
    public:
        AddCommand(
            std::weak_ptr<GameRepositoryInterface> gameRepository,
            std::weak_ptr<Output> output,
            std::string gameName,
            std::string path
        ) {
            this->gameRepository = gameRepository;
            this->output = output;
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
                    if (auto out = output.lock()) {
                        out->printMessage("Game already exist");
                    }
                }
                else {
                    gr->addGame(gameName, temp);
                }
            }
            
        }
};