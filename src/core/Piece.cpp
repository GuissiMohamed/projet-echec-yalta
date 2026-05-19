#include "core/Piece.hpp"

#include <array>

#include "core/Board.hpp"
#include "core/Player.hpp"
#include "core/Square.hpp"

namespace
{
    int stepOrthogonal(const Board& board, int squareId, OrthogonalDirection direction)
    {
        return board.getOrthogonalNeighbor(squareId, direction);
    }

    int stepDiagonal(const Board& board, int squareId, DiagonalDirection direction)
    {
        return board.getDiagonalNeighbor(squareId, direction);
    }

    int stepKnightPath(
        const Board& board,
        int squareId,
        OrthogonalDirection firstA,
        OrthogonalDirection firstB,
        OrthogonalDirection turn)
    {
        int step1 = board.getOrthogonalNeighbor(squareId, firstA);
        if (step1 < 0)
        {
            return -1;
        }

        int step2 = board.getOrthogonalNeighbor(step1, firstB);
        if (step2 < 0)
        {
            return -1;
        }

        return board.getOrthogonalNeighbor(step2, turn);
    }
}

Piece::Piece(PieceType type, Player* owner, Square* square, Board* board)
    : m_type(type),
      m_owner(owner),
      m_square(square),
      m_board(board),
      m_hasMoved(false),
      m_isCaptured(false)
{
}

PieceType Piece::getType() const
{
    return m_type;
}

void Piece::setType(PieceType type)
{
    m_type = type;
}

Player* Piece::getOwner() const
{
    return m_owner;
}

Square* Piece::getSquare() const
{
    return m_square;
}

Board* Piece::getBoard() const
{
    return m_board;
}

void Piece::setSquare(Square* square)
{
    m_square = square;
}

bool Piece::hasMoved() const
{
    return m_hasMoved;
}

void Piece::setHasMoved(bool hasMoved)
{
    m_hasMoved = hasMoved;
}

bool Piece::isCaptured() const
{
    return m_isCaptured;
}

void Piece::setCaptured(bool captured)
{
    m_isCaptured = captured;
}

void Piece::addSlidingMoves(std::vector<Move>& moves, const int directions[][2], int directionCount) const
{
    // Garde-fou : cette ancienne signature n'est plus utilisée par la nouvelle logique.
    (void)moves;
    (void)directions;
    (void)directionCount;
}

std::vector<Move> Piece::getPseudoLegalMoves() const
{
    std::vector<Move> moves;

    if (!m_square || !m_board || !m_owner || m_isCaptured)
    {
        return moves;
    }

    const int fromId = m_square->getId();

    auto addDestination = [this, &moves, fromId](int targetId)
    {
        if (targetId < 0)
        {
            return;
        }

        Square* target = m_board->getSquare(targetId);
        if (target == nullptr)
        {
            return;
        }

        if (!target->isOccupied())
        {
            moves.emplace_back(fromId, targetId, MoveType::Normal);
        }
        else if (target->getPiece() != nullptr && target->getPiece()->getOwner() != m_owner)
        {
            moves.emplace_back(fromId, targetId, MoveType::Capture);
        }
    };

    auto addSlidingLineOrthogonal = [this, &moves, fromId](OrthogonalDirection direction)
    {
        int current = m_board->getOrthogonalNeighbor(fromId, direction);

        while (current >= 0)
        {
            Square* target = m_board->getSquare(current);
            if (target == nullptr)
            {
                break;
            }

            if (!target->isOccupied())
            {
                moves.emplace_back(fromId, current, MoveType::Normal);
            }
            else
            {
                if (target->getPiece() != nullptr && target->getPiece()->getOwner() != m_owner)
                {
                    moves.emplace_back(fromId, current, MoveType::Capture);
                }
                break;
            }

            current = m_board->getOrthogonalNeighbor(current, direction);
        }
    };

    auto addSlidingLineDiagonal = [this, &moves, fromId](DiagonalDirection direction)
    {
        int current = m_board->getDiagonalNeighbor(fromId, direction);

        while (current >= 0)
        {
            Square* target = m_board->getSquare(current);
            if (target == nullptr)
            {
                break;
            }

            if (!target->isOccupied())
            {
                moves.emplace_back(fromId, current, MoveType::Normal);
            }
            else
            {
                if (target->getPiece() != nullptr && target->getPiece()->getOwner() != m_owner)
                {
                    moves.emplace_back(fromId, current, MoveType::Capture);
                }
                break;
            }

            current = m_board->getDiagonalNeighbor(current, direction);
        }
    };

    if (m_type == PieceType::King)
    {
        const std::array<OrthogonalDirection, 4> orthogonals = {
            OrthogonalDirection::Up,
            OrthogonalDirection::Down,
            OrthogonalDirection::Left,
            OrthogonalDirection::Right
        };

        const std::array<DiagonalDirection, 4> diagonals = {
            DiagonalDirection::UpLeft,
            DiagonalDirection::UpRight,
            DiagonalDirection::DownLeft,
            DiagonalDirection::DownRight
        };

        for (OrthogonalDirection direction : orthogonals)
        {
            addDestination(stepOrthogonal(*m_board, fromId, direction));
        }

        for (DiagonalDirection direction : diagonals)
        {
            addDestination(stepDiagonal(*m_board, fromId, direction));
        }

        return moves;
    }

    if (m_type == PieceType::Rook)
    {
        addSlidingLineOrthogonal(OrthogonalDirection::Up);
        addSlidingLineOrthogonal(OrthogonalDirection::Down);
        addSlidingLineOrthogonal(OrthogonalDirection::Left);
        addSlidingLineOrthogonal(OrthogonalDirection::Right);
        return moves;
    }

    if (m_type == PieceType::Bishop)
    {
        addSlidingLineDiagonal(DiagonalDirection::UpLeft);
        addSlidingLineDiagonal(DiagonalDirection::UpRight);
        addSlidingLineDiagonal(DiagonalDirection::DownLeft);
        addSlidingLineDiagonal(DiagonalDirection::DownRight);
        return moves;
    }

    if (m_type == PieceType::Queen)
    {
        addSlidingLineOrthogonal(OrthogonalDirection::Up);
        addSlidingLineOrthogonal(OrthogonalDirection::Down);
        addSlidingLineOrthogonal(OrthogonalDirection::Left);
        addSlidingLineOrthogonal(OrthogonalDirection::Right);

        addSlidingLineDiagonal(DiagonalDirection::UpLeft);
        addSlidingLineDiagonal(DiagonalDirection::UpRight);
        addSlidingLineDiagonal(DiagonalDirection::DownLeft);
        addSlidingLineDiagonal(DiagonalDirection::DownRight);
        return moves;
    }

    if (m_type == PieceType::Knight)
    {
        const std::array<int, 8> knightTargets = {
            stepKnightPath(*m_board, fromId, OrthogonalDirection::Up, OrthogonalDirection::Up, OrthogonalDirection::Left),
            stepKnightPath(*m_board, fromId, OrthogonalDirection::Up, OrthogonalDirection::Up, OrthogonalDirection::Right),
            stepKnightPath(*m_board, fromId, OrthogonalDirection::Down, OrthogonalDirection::Down, OrthogonalDirection::Left),
            stepKnightPath(*m_board, fromId, OrthogonalDirection::Down, OrthogonalDirection::Down, OrthogonalDirection::Right),
            stepKnightPath(*m_board, fromId, OrthogonalDirection::Left, OrthogonalDirection::Left, OrthogonalDirection::Up),
            stepKnightPath(*m_board, fromId, OrthogonalDirection::Left, OrthogonalDirection::Left, OrthogonalDirection::Down),
            stepKnightPath(*m_board, fromId, OrthogonalDirection::Right, OrthogonalDirection::Right, OrthogonalDirection::Up),
            stepKnightPath(*m_board, fromId, OrthogonalDirection::Right, OrthogonalDirection::Right, OrthogonalDirection::Down)
        };

        for (int targetId : knightTargets)
        {
            addDestination(targetId);
        }

        return moves;
    }

    if (m_type == PieceType::Pawn)
    {
        const OrthogonalDirection forward = m_owner->getPawnForwardDirection();
        const auto [captureA, captureB] = m_owner->getPawnCaptureDirections();

        const int oneStep = m_board->getOrthogonalNeighbor(fromId, forward);
        if (oneStep >= 0)
        {
            Square* oneStepSquare = m_board->getSquare(oneStep);
            if (oneStepSquare != nullptr && !oneStepSquare->isOccupied())
            {
                moves.emplace_back(fromId, oneStep, MoveType::Normal);

                if (!m_hasMoved)
                {
                    const int twoStep = m_board->getOrthogonalNeighbor(oneStep, forward);
                    if (twoStep >= 0)
                    {
                        Square* twoStepSquare = m_board->getSquare(twoStep);
                        if (twoStepSquare != nullptr && !twoStepSquare->isOccupied())
                        {
                            moves.emplace_back(fromId, twoStep, MoveType::Normal);
                        }
                    }
                }
            }
        }

        const std::array<DiagonalDirection, 2> captureDirections = {captureA, captureB};
        for (DiagonalDirection direction : captureDirections)
        {
            const int targetId = m_board->getDiagonalNeighbor(fromId, direction);
            if (targetId < 0)
            {
                continue;
            }

            Square* target = m_board->getSquare(targetId);
            if (target != nullptr && target->isOccupied() && target->getPiece()->getOwner() != m_owner)
            {
                moves.emplace_back(fromId, targetId, MoveType::Capture);
            }
        }

        return moves;
    }

    return moves;
}