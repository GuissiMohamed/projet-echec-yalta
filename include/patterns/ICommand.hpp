 /**
 * @class MoveCommand
 * @brief Implémentation du pattern Command
 *
 * Encapsule un coup à exécuter sur le jeu.
 */

#pragma once

#include <string>

class ICommand
{
public:
    virtual ~ICommand() = default;

    virtual bool execute() = 0;
    virtual std::string getLabel() const = 0;
};