#pragma once

#include <vector>

#include "core/Game.hpp"
#include "core/Move.hpp"

class SFMLView;
class Square;

class SFMLGameController
{
public:
    SFMLGameController(Game& game, SFMLView& view);

    void setupDefaultPosition();
    void run();

private:
    void placeHorizontalArmy(PlayerColor color, int backRow, int pawnRow);
    void placeVerticalArmy(PlayerColor color, int backCol, int pawnCol);

    void resetSelection();
    void selectSquare(Square* square);
    void handleBoardClick(int squareId);
    bool tryPlayMoveTo(int targetSquareId);

    Game& m_game;
    SFMLView& m_view;

    Square* m_selectedSquare;
    std::vector<Move> m_selectedMoves;
};