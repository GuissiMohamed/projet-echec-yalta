#include "core/Piece.hpp"

#include "core/Board.hpp"
#include "core/Player.hpp"
#include "core/Square.hpp"

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
    if (!m_square || !m_board || !m_owner)
    {
        return;
    }

    const int row = m_board->getRow(m_square->getId());
    const int col = m_board->getCol(m_square->getId());

    for (int i = 0; i < directionCount; ++i)
    {
        int currentRow = row;
        int currentCol = col;

        while (true)
        {
            currentRow += directions[i][0];
            currentCol += directions[i][1];

            if (!m_board->isValid(currentRow, currentCol))
            {
                break;
            }

            Square* target = m_board->getSquare(currentRow, currentCol);
            if (target == nullptr)
            {
                break;
            }

            if (!target->isOccupied())
            {
                moves.emplace_back(m_square, target, MoveType::Normal);
                continue;
            }

            if (target->getPiece()->getOwner() != m_owner)
            {
                moves.emplace_back(m_square, target, MoveType::Capture);
            }

            break;
        }
    }
}

std::vector<Move> Piece::getPseudoLegalMoves() const
{
    std::vector<Move> moves;

    if (!m_square || !m_board || !m_owner || m_isCaptured)
    {
        return moves;
    }

    const int id = m_square->getId();
    const int row = m_board->getRow(id);
    const int col = m_board->getCol(id);

    if (m_type == PieceType::King)
    {
        for (int dr = -1; dr <= 1; ++dr)
        {
            for (int dc = -1; dc <= 1; ++dc)
            {
                if (dr == 0 && dc == 0)
                {
                    continue;
                }

                const int nextRow = row + dr;
                const int nextCol = col + dc;

                if (!m_board->isValid(nextRow, nextCol))
                {
                    continue;
                }

                Square* target = m_board->getSquare(nextRow, nextCol);
                if (target == nullptr)
                {
                    continue;
                }

                if (!target->isOccupied())
                {
                    moves.emplace_back(m_square, target, MoveType::Normal);
                }
                else if (target->getPiece()->getOwner() != m_owner)
                {
                    moves.emplace_back(m_square, target, MoveType::Capture);
                }
            }
        }

        return moves;
    }

    if (m_type == PieceType::Rook)
    {
        const int directions[4][2] = {
            {-1, 0},
            {1, 0},
            {0, -1},
            {0, 1}
        };

        addSlidingMoves(moves, directions, 4);
        return moves;
    }

    if (m_type == PieceType::Bishop)
    {
        const int directions[4][2] = {
            {-1, -1},
            {-1, 1},
            {1, -1},
            {1, 1}
        };

        addSlidingMoves(moves, directions, 4);
        return moves;
    }

    if (m_type == PieceType::Queen)
    {
        const int directions[8][2] = {
            {-1, 0}, {1, 0}, {0, -1}, {0, 1},
            {-1, -1}, {-1, 1}, {1, -1}, {1, 1}
        };

        addSlidingMoves(moves, directions, 8);
        return moves;
    }

    if (m_type == PieceType::Knight)
    {
        const int offsets[8][2] = {
            {-2, -1}, {-2, 1},
            {-1, -2}, {-1, 2},
            {1, -2}, {1, 2},
            {2, -1}, {2, 1}
        };

        for (const auto& offset : offsets)
        {
            const int nextRow = row + offset[0];
            const int nextCol = col + offset[1];

            if (!m_board->isValid(nextRow, nextCol))
            {
                continue;
            }

            Square* target = m_board->getSquare(nextRow, nextCol);
            if (target == nullptr)
            {
                continue;
            }

            if (!target->isOccupied())
            {
                moves.emplace_back(m_square, target, MoveType::Normal);
            }
            else if (target->getPiece()->getOwner() != m_owner)
            {
                moves.emplace_back(m_square, target, MoveType::Capture);
            }
        }

        return moves;
    }

    if (m_type == PieceType::Pawn)
    {
        const auto [forwardRow, forwardCol] = m_owner->getForwardDirection();
        const int oneStepRow = row + forwardRow;
        const int oneStepCol = col + forwardCol;

        if (m_board->isValid(oneStepRow, oneStepCol))
        {
            Square* oneStepTarget = m_board->getSquare(oneStepRow, oneStepCol);
            if (oneStepTarget != nullptr && !oneStepTarget->isOccupied())
            {
                moves.emplace_back(m_square, oneStepTarget, MoveType::Normal);

                const int twoStepRow = row + (2 * forwardRow);
                const int twoStepCol = col + (2 * forwardCol);

                if (!m_hasMoved && m_board->isValid(twoStepRow, twoStepCol))
                {
                    Square* twoStepTarget = m_board->getSquare(twoStepRow, twoStepCol);
                    if (twoStepTarget != nullptr && !twoStepTarget->isOccupied())
                    {
                        moves.emplace_back(m_square, twoStepTarget, MoveType::Normal);
                    }
                }
            }
        }

        const auto captureDirections = m_owner->getPawnCaptureDirections();
        for (const auto& [captureRowDelta, captureColDelta] : captureDirections)
        {
            const int captureRow = row + captureRowDelta;
            const int captureCol = col + captureColDelta;

            if (!m_board->isValid(captureRow, captureCol))
            {
                continue;
            }

            Square* target = m_board->getSquare(captureRow, captureCol);
            if (target != nullptr && target->isOccupied() && target->getPiece()->getOwner() != m_owner)
            {
                moves.emplace_back(m_square, target, MoveType::Capture);
            }
        }

        return moves;
    }

    return moves;
}