#pragma once

#include <vector>

#include "core/Game.hpp"
#include "core/Move.hpp"

class ConsoleView;
class Piece;

class GameController
{
public:
    GameController(Game& game, ConsoleView& view);

    void setupDefaultPosition();
    void run();

private:
    bool isSessionFinished() const;
    bool handleTurn();

    std::vector<Move> filterMovesByFrom(const std::vector<Move>& moves, int fromId) const;
    std::vector<Move> filterMovesByTo(const std::vector<Move>& moves, int toId) const;
    Move selectPromotionMove(const std::vector<Move>& promotionMoves) const;

    void placeHorizontalArmy(PlayerColor color, int backRow, int pawnRow);
    void placeVerticalArmy(PlayerColor color, int backCol, int pawnCol);

    Game& m_game;
    ConsoleView& m_view;
    bool m_userRequestedQuit;
};