#pragma once

#include "core/Move.hpp"
#include "core/Types.hpp"

class Game;

class AlphaBetaAI
{
public:
    explicit AlphaBetaAI(int depth = 2);

    Move chooseBestMove(const Game& game, PlayerColor aiColor) const;

private:
    int alphaBeta(Game& game, int depth, PlayerColor aiColor, int alpha, int beta) const;
    int evaluate(Game& game, PlayerColor aiColor) const;
    int pieceValue(PieceType type) const;

    int m_depth;
};