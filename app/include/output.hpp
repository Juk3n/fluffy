#pragma once

#include <iostream>
#include <string>

class Output {
private:
    bool debugRun = false;
public:
    auto turnOnDebug() -> void {
        debugRun = true;
        printDebugMessage("Debug mode on");
    }

    auto printDebugMessage(std::string message) -> void {
        if (debugRun)
            std::cout << message << std::endl;
    }
    auto printDebugError(std::string message) -> void {
        if (debugRun)
            std::cerr << message << std::endl;
    }
    auto printMessage(std::string message) -> void {
        std::cout << message << std::endl;
    }
};
