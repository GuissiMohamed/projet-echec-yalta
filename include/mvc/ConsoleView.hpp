#pragma once

#include <string>
#include <vector>

#include "core/Game.hpp"
#include "core/Move.hpp"

class ConsoleView
{
public:
    void showWelcome() const;
    void showBoard(const Game& game) const;
    void showStatus(const Game& game) const;
    void showMoves(const std::vector<Move>& moves) const;
    void showMessage(const std::string& message) const;
    void showEnd(const Game& game) const;

    std::string askCommand() const;
    int askSquareId(const std::string& prompt) const;
    int askPromotionChoice() const;
};