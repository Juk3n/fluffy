#include "fluffyApplication.hpp"

#include <iostream>

auto FluffyApplication::runGame(std::string gameName) -> void {
    std::string pathToRun = gameRepository->getGameByName(gameName).getPath();
    if(pathToRun.empty()) {
        std::cout << "no application found" << std::endl;
    }
    else {
        system(pathToRun.c_str());
    }
}

auto FluffyApplication::handleCommand(
    const std::string& command,
    const std::vector<std::string>& arguments
) -> void {
    if (command == "show") {
        ShowCommand showCommand = ShowCommand(gameRepository);
        showCommand.execute();
    }
    else if (command == "rm") {
        RemoveCommand removeCommand = RemoveCommand(gameRepository, arguments[0]);
        removeCommand.execute();
    }
    else if (command == "add") {
        std::string gameName = arguments[0];
        std::string gamePath = arguments[1];
        
        this->addGame(gameName, gamePath);
    }
    else if (command == "play") {
        std::string gameName{arguments[0]};
        PlayCommand playCommand = PlayCommand(gameRepository, gameName);
        playCommand.execute();
    }
    else if (command == "--version") {
        output->printMessage("fluffy " + version);
    }
    else if (command == "--help") {
        output->printMessage(this->helpMessage);
    }
    else {
        output->printMessage("fluffy: '" + command + "' is not a fluffy command. See 'fluffy --help'");
    }
}

auto FluffyApplication::addGame(std::string name, std::string path) -> void {
    std::string gameName{name};
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
    Game currentGame = gameRepository->getGameByName(gameName);
    if (currentGame.getName().length() != 0) {
        output->printMessage("Game already exist");
    }
    else {
        gameRepository->addGame(gameName, temp);
    }

}

auto FluffyApplication::handleFlags(std::vector<std::string> flags) -> void {
    for (const auto& flag : flags) {
        if (flag == "-d") {
            output->turnOnDebug();
        }
        else {
            output->printMessage("unkown option: '" + flag + "'");
        }
    }
}

auto FluffyApplication::getExecutablePath() -> std::filesystem::path {
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    return std::filesystem::path(std::string(result, (count > 0) ? count : 0));
}

auto FluffyApplication::parseFlags(int argc, char const *argv[]) -> std::vector<std::string> {
    std::vector<std::string> flags;
    for (int i = 1; i < argc; i ++) {
        std::string appArgument = argv[i];
        if (appArgument.size() < 2) continue;
        if (appArgument[0] == '-' and appArgument[1] != '-') flags.emplace_back(appArgument);
    }
    return flags;
}

auto FluffyApplication::parseCommand(int argc, char const* argv[]) {
    std::string command{};
    std::vector<std::string> arguments{};
    for (int i = 1; i < argc; i ++) {
        std::string appArgument = argv[i];
        if (appArgument.size() < 2) continue;
        if ((appArgument[0] == '-' and appArgument[1] == '-') or appArgument[0] != '-') {
        if (command == "") command = appArgument;
        else arguments.emplace_back(appArgument);
        }
    }
    struct Command { std::string command; std::vector<std::string> arguments; };
    return Command{command, arguments};
}

FluffyApplication::FluffyApplication(int argc, char const *argv[]) {
    output = std::make_shared<Output>();

    auto databasePath{ std::filesystem::path(getExecutablePath().parent_path().string() + "/data.db") };
    
    gameRepository = std::make_shared<GameRepository>(
        std::make_unique<Database>(databasePath, output)
    );

    auto flags = parseFlags(argc, argv);
    handleFlags(flags);
    if (argc - flags.size() > 1) {
      auto [command, arguments] = parseCommand(argc, argv);
      if (command != "") handleCommand(command, arguments);
    }
    else {
        handleCommand("--help", {});
    }    
}
