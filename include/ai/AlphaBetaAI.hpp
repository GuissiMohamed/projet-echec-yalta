#pragma once

#include <cstddef>
#include <vector>

#include "core/Move.hpp"
#include "core/Types.hpp"

class Game;
class Piece;

class AlphaBetaAI
{
public:
    explicit AlphaBetaAI(int depth = 2, unsigned int threadCount = 0);

    Move chooseBestMove(const Game& game, PlayerColor aiColor) const;

    int getDepth() const;
    unsigned int getThreadCount() const;

private:
    struct MoveScore
    {
        Move move;
        int score = 0;
        bool valid = false;
    };

    MoveScore evaluateRootChunk(
        const Game& game,
        const std::vector<Move>& moves,
        std::size_t begin,
        std::size_t end,
        PlayerColor aiColor) const;

    int alphaBeta(Game& game, int depth, PlayerColor aiColor, int alpha, int beta) const;
    int evaluate(Game& game, PlayerColor aiColor) const;
    int pieceValue(PieceType type) const;

    int staticMoveScore(Game& game, const Move& move, PlayerColor aiColor) const;
    void orderMoves(Game& game, std::vector<Move>& moves, PlayerColor aiColor) const;

    int pawnAdvanceBonus(const Game& game, const Piece& piece) const;

    int m_depth;
    unsigned int m_threadCount;
};