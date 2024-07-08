# pragma once

#include <iostream>
#include <string>

class Output {
public:
    auto printDebugMessage(std::string message) -> void {
        std::cout << message << std::endl;
    }
};