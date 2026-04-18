#pragma once

#include <string>

class ICommand
{
public:
    virtual ~ICommand() = default;

    virtual bool execute() = 0;
    virtual std::string getLabel() const = 0;
};