#pragma once

#include <string>

#include "patterns/ICommand.hpp"
#include "core/Move.hpp"

class Game;

class MoveCommand : public ICommand
{
public:
    MoveCommand(Game& game, const Move& move);

    bool execute() override;
    std::string getLabel() const override;

private:
    Game& m_game;
    Move m_move;
};