#pragma once

#include <memory>

#include <command.hpp>
#include <gameRepository.hpp>
#include <output.hpp>

class PlayCommand: public Command {
    private:
        std::weak_ptr<GameRepositoryInterface> gameRepository;
        std::weak_ptr<Output> output;
        std::string gameName;
    
    public:
        PlayCommand(
            std::weak_ptr<GameRepositoryInterface> gameRepository,
            std::weak_ptr<Output> output,
            std::string gameName
        ) {
            this->gameRepository = gameRepository;
            this->output = output;
            this->gameName = gameName;
        }

        void execute() override {
            if (std::shared_ptr<GameRepositoryInterface> gr = gameRepository.lock()) {
                std::string pathToRun = gr->getGameByName(gameName).getPath();
                if(pathToRun.empty()) {
                    if (auto out = output.lock()) {
                        out->printMessage("No application found");
                    }
                }
                else {
                    system(pathToRun.c_str());
                }
            }
        }
};