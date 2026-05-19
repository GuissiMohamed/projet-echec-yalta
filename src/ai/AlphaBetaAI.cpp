#include "ai/AlphaBetaAI.hpp"

#include <algorithm>
#include <future>
#include <limits>
#include <thread>
#include <vector>

#include "core/Game.hpp"
#include "core/Piece.hpp"
#include "core/Player.hpp"
#include "core/Square.hpp"

namespace
{
    constexpr int kInfinity = 100000000;

    const PlayerColor kColors[3] = {
        PlayerColor::Red,
        PlayerColor::Blue,
        PlayerColor::Green
    };

    unsigned int resolveThreadCount(unsigned int requested)
    {
        if (requested > 0)
        {
            return requested;
        }

        const unsigned int detected = std::thread::hardware_concurrency();
        return (detected == 0) ? 1u : detected;
    }
}

AlphaBetaAI::AlphaBetaAI(int depth, unsigned int threadCount)
    : m_depth(depth),
      m_threadCount(resolveThreadCount(threadCount))
{
}

int AlphaBetaAI::getDepth() const
{
    return m_depth;
}

unsigned int AlphaBetaAI::getThreadCount() const
{
    return m_threadCount;
}

Move AlphaBetaAI::chooseBestMove(const Game& originalGame, PlayerColor aiColor) const
{
    Game root(originalGame);

    Player* currentPlayer = root.getCurrentPlayer();
    if (currentPlayer == nullptr || currentPlayer->getColor() != aiColor)
    {
        return {};
    }

    std::vector<Move> legalMoves = root.getLegalMovesForPlayer(aiColor);
    if (legalMoves.empty())
    {
        return {};
    }

    orderMoves(root, legalMoves, aiColor);

    const std::size_t workerCount = std::min<std::size_t>(
        static_cast<std::size_t>(m_threadCount),
        legalMoves.size());

    if (workerCount <= 1)
    {
        MoveScore best = evaluateRootChunk(root, legalMoves, 0, legalMoves.size(), aiColor);
        return best.valid ? best.move : legalMoves.front();
    }

    const std::size_t chunkSize = (legalMoves.size() + workerCount - 1) / workerCount;
    std::vector<std::future<MoveScore>> futures;
    futures.reserve(workerCount);

    for (std::size_t worker = 0; worker < workerCount; ++worker)
    {
        const std::size_t begin = worker * chunkSize;
        const std::size_t end = std::min(begin + chunkSize, legalMoves.size());

        if (begin >= end)
        {
            continue;
        }

        futures.emplace_back(
            std::async(
                std::launch::async,
                [this, &root, &legalMoves, begin, end, aiColor]() -> MoveScore
                {
                    return evaluateRootChunk(root, legalMoves, begin, end, aiColor);
                }));
    }

    MoveScore best;
    best.score = -kInfinity;
    best.valid = false;

    for (auto& future : futures)
    {
        const MoveScore candidate = future.get();

        if (!candidate.valid)
        {
            continue;
        }

        if (!best.valid || candidate.score > best.score)
        {
            best = candidate;
        }
    }

    return best.valid ? best.move : legalMoves.front();
}

AlphaBetaAI::MoveScore AlphaBetaAI::evaluateRootChunk(
    const Game& originalGame,
    const std::vector<Move>& moves,
    std::size_t begin,
    std::size_t end,
    PlayerColor aiColor) const
{
    MoveScore best;
    best.score = -kInfinity;
    best.valid = false;

    for (std::size_t i = begin; i < end; ++i)
    {
        const Move& move = moves[i];

        Game child(originalGame);
        if (!child.playMove(move))
        {
            continue;
        }

        const int score = alphaBeta(child, m_depth - 1, aiColor, -kInfinity, kInfinity);

        if (!best.valid || score > best.score)
        {
            best.move = move;
            best.score = score;
            best.valid = true;
        }
    }

    return best;
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
    std::vector<Move> legalMoves = game.getLegalMovesForPlayer(currentColor);

    if (legalMoves.empty())
    {
        return evaluate(game, aiColor);
    }

    orderMoves(game, legalMoves, aiColor);

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
            value = std::max(value, score);
            alpha = std::max(alpha, value);

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
        value = std::min(value, score);
        beta = std::min(beta, value);

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
        return (game.getWinner() == aiColor) ? 500000 : -500000;
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

        const int sign = (color == aiColor) ? 1 : -1;

        if (player->isEliminated())
        {
            score += sign * (color == aiColor ? -30000 : 15000);
            continue;
        }

        for (Piece* piece : player->getPieces())
        {
            if (piece == nullptr || piece->isCaptured())
            {
                continue;
            }

            score += sign * pieceValue(piece->getType());

            if (piece->getType() == PieceType::Pawn)
            {
                score += sign * pawnAdvanceBonus(game, *piece);
            }
        }

        const int mobility = static_cast<int>(game.getLegalMovesForPlayer(color).size());
        score += sign * (mobility * 4);

        if (game.isPlayerInCheck(color))
        {
            score += sign * (color == aiColor ? -400 : 220);
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

int AlphaBetaAI::staticMoveScore(Game& game, const Move& move, PlayerColor aiColor) const
{
    int score = 0;

    switch (move.getType())
    {
        case MoveType::Promotion:
            score += 10000;
            score += pieceValue(move.getPromotionType());
            break;
        case MoveType::Capture:
            score += 3000;
            break;
        case MoveType::EnPassant:
            score += 2800;
            break;
        case MoveType::Castling:
            score += 1200;
            break;
        case MoveType::Normal:
        default:
            break;
    }

    Square* fromSquare = move.resolveFrom(game.getBoard());
    Square* toSquare = move.resolveTo(game.getBoard());

    if (fromSquare != nullptr && fromSquare->isOccupied())
    {
        Piece* movingPiece = fromSquare->getPiece();
        if (movingPiece != nullptr)
        {
            score += pieceValue(movingPiece->getType()) / 20;

            if (movingPiece->getType() == PieceType::Pawn)
            {
                score += pawnAdvanceBonus(game, *movingPiece) / 2;
            }
        }
    }

    if (toSquare != nullptr && toSquare->isOccupied())
    {
        Piece* capturedPiece = toSquare->getPiece();
        if (capturedPiece != nullptr)
        {
            score += pieceValue(capturedPiece->getType());
        }
    }

    if (move.getType() == MoveType::Promotion && aiColor != PlayerColor::None)
    {
        score += 250;
    }

    return score;
}

void AlphaBetaAI::orderMoves(Game& game, std::vector<Move>& moves, PlayerColor aiColor) const
{
    std::stable_sort(
        moves.begin(),
        moves.end(),
        [this, &game, aiColor](const Move& left, const Move& right)
        {
            return staticMoveScore(game, left, aiColor) > staticMoveScore(game, right, aiColor);
        });
}

int AlphaBetaAI::pawnAdvanceBonus(const Game& game, const Piece& piece) const
{
    if (piece.getSquare() == nullptr || piece.getOwner() == nullptr)
    {
        return 0;
    }

    const int row = game.getBoard().getRow(piece.getSquare()->getId());
    const int col = game.getBoard().getCol(piece.getSquare()->getId());

    switch (piece.getOwner()->getColor())
    {
        case PlayerColor::Red:
            return (7 - row) * 8;
        case PlayerColor::Blue:
            return row * 8;
        case PlayerColor::Green:
            return (11 - col) * 8;
        default:
            return 0;
    }
}