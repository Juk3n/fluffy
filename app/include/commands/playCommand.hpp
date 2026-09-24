#pragma once

#include <memory>

#include <command.hpp>
#include <gameRepository.hpp>

class PlayCommand: public Command {
    private:
        std::weak_ptr<GameRepositoryInterface> gameRepository;
        std::string gameName;
    
    public:
        PlayCommand(
            std::weak_ptr<GameRepositoryInterface> gameRepository,
            std::string gameName
        ) {
            this->gameRepository = gameRepository;
            this->gameName = gameName;
        }

        void execute() override {
            if (std::shared_ptr<GameRepositoryInterface> gr = gameRepository.lock()) {
                std::string pathToRun = gr->getGameByName(gameName).getPath();
                if(pathToRun.empty()) {
                    std::cout << "no application found" << std::endl;
                }
                else {
                    system(pathToRun.c_str());
                }
            }
        }
};