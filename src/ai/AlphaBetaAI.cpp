#include "ai/AlphaBetaAI.hpp"

#include <limits>

#include "core/Game.hpp"
#include "core/Piece.hpp"
#include "core/Player.hpp"

namespace
{
    constexpr int kInfinity = 100000000;

    const PlayerColor kColors[3] = {
        PlayerColor::Red,
        PlayerColor::Blue,
        PlayerColor::Green
    };
}

AlphaBetaAI::AlphaBetaAI(int depth)
    : m_depth(depth)
{
}

Move AlphaBetaAI::chooseBestMove(const Game& originalGame, PlayerColor aiColor) const
{
    Game root(originalGame);

    const std::vector<Move> legalMoves = root.getLegalMovesForPlayer(aiColor);
    if (legalMoves.empty())
    {
        return {};
    }

    Move bestMove = legalMoves.front();
    int bestScore = -kInfinity;

    for (const Move& move : legalMoves)
    {
        Game child(root);

        if (!child.playMove(move))
        {
            continue;
        }

        const int score = alphaBeta(child, m_depth - 1, aiColor, -kInfinity, kInfinity);

        if (score > bestScore)
        {
            bestScore = score;
            bestMove = move;
        }
    }

    return bestMove;
}

int AlphaBetaAI::alphaBeta(Game& game, int depth, PlayerColor aiColor, int alpha, int beta) const
{
    const GameState state = game.getState();

    if (depth <= 0
        || state == GameState::Draw
        || state == GameState::Stalemate
        || state == GameState::WinByElimination)
    {
        return evaluate(game, aiColor);
    }

    Player* currentPlayer = game.getCurrentPlayer();
    if (currentPlayer == nullptr)
    {
        return evaluate(game, aiColor);
    }

    const PlayerColor currentColor = currentPlayer->getColor();
    const std::vector<Move> legalMoves = game.getLegalMovesForPlayer(currentColor);

    if (legalMoves.empty())
    {
        return evaluate(game, aiColor);
    }

    const bool maximizing = (currentColor == aiColor);

    if (maximizing)
    {
        int value = -kInfinity;

        for (const Move& move : legalMoves)
        {
            Game child(game);

            if (!child.playMove(move))
            {
                continue;
            }

            const int score = alphaBeta(child, depth - 1, aiColor, alpha, beta);
            if (score > value)
            {
                value = score;
            }

            if (value > alpha)
            {
                alpha = value;
            }

            if (beta <= alpha)
            {
                break;
            }
        }

        return value;
    }

    int value = kInfinity;

    for (const Move& move : legalMoves)
    {
        Game child(game);

        if (!child.playMove(move))
        {
            continue;
        }

        const int score = alphaBeta(child, depth - 1, aiColor, alpha, beta);
        if (score < value)
        {
            value = score;
        }

        if (value < beta)
        {
            beta = value;
        }

        if (beta <= alpha)
        {
            break;
        }
    }

    return value;
}

int AlphaBetaAI::evaluate(Game& game, PlayerColor aiColor) const
{
    if (game.getState() == GameState::WinByElimination)
    {
        if (game.getWinner() == aiColor)
        {
            return 500000;
        }

        return -500000;
    }

    if (game.getState() == GameState::Draw || game.getState() == GameState::Stalemate)
    {
        return 0;
    }

    int score = 0;

    for (PlayerColor color : kColors)
    {
        Player* player = game.getPlayer(color);
        if (player == nullptr)
        {
            continue;
        }

        int sign = (color == aiColor) ? 1 : -1;

        if (player->isEliminated())
        {
            score += sign * (color == aiColor ? -20000 : 12000);
            continue;
        }

        for (Piece* piece : player->getPieces())
        {
            if (piece == nullptr || piece->isCaptured())
            {
                continue;
            }

            score += sign * pieceValue(piece->getType());
        }

        const int mobility = static_cast<int>(game.getLegalMovesForPlayer(color).size());
        score += sign * (mobility * 3);

        if (game.isPlayerInCheck(color))
        {
            score += sign * (color == aiColor ? -250 : 180);
        }
    }

    return score;
}

int AlphaBetaAI::pieceValue(PieceType type) const
{
    switch (type)
    {
        case PieceType::Pawn:   return 100;
        case PieceType::Knight: return 320;
        case PieceType::Bishop: return 330;
        case PieceType::Rook:   return 500;
        case PieceType::Queen:  return 900;
        case PieceType::King:   return 20000;
        default:                return 0;
    }
}