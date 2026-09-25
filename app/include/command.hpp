#pragma once

#include <memory>
#include <iostream>

#include <gameRepository.hpp>

class Command {
    public:
        virtual void execute() = 0;
        virtual ~Command() {}
};