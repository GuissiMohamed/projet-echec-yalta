#include "core/Piece.hpp"
#include "core/Board.hpp"
#include "core/Player.hpp"
#include "core/Square.hpp"

Piece::Piece(PieceType type, Player* owner, Square* square, Board* board)
    : m_type(type), m_owner(owner), m_square(square), m_board(board)
{
}

PieceType Piece::getType() const
{
    return m_type;
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

std::vector<Move> Piece::getLegalMoves() const
{
    std::vector<Move> moves;

    if (!m_square || !m_board)
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

                const int nr = row + dr;
                const int nc = col + dc;

                if (m_board->isValid(nr, nc))
                {
                    Square* target = m_board->getSquare(m_board->toIndex(nr, nc));

                    if (!target->isOccupied() || target->getPiece()->getOwner() != m_owner)
                    {
                        moves.emplace_back(m_square, target);
                    }
                }
            }
        }
    }

    if (m_type == PieceType::Rook)
    {
        const int directions[4][2] = {
            {-1, 0},
            {1, 0},
            {0, -1},
            {0, 1}
        };

        for (const auto& direction : directions)
        {
            int nr = row;
            int nc = col;

            while (true)
            {
                nr += direction[0];
                nc += direction[1];

                if (!m_board->isValid(nr, nc))
                {
                    break;
                }

                Square* target = m_board->getSquare(m_board->toIndex(nr, nc));

                if (!target->isOccupied())
                {
                    moves.emplace_back(m_square, target);
                }
                else
                {
                    if (target->getPiece()->getOwner() != m_owner)
                    {
                        moves.emplace_back(m_square, target);
                    }
                    break;
                }
            }
        }
    }

    if (m_type == PieceType::Bishop)
    {
        const int directions[4][2] = {
            {-1, -1},
            {-1, 1},
            {1, -1},
            {1, 1}
        };

        for (const auto& direction : directions)
        {
            int nr = row;
            int nc = col;

            while (true)
            {
                nr += direction[0];
                nc += direction[1];

                if (!m_board->isValid(nr, nc))
                {
                    break;
                }

                Square* target = m_board->getSquare(m_board->toIndex(nr, nc));

                if (!target->isOccupied())
                {
                    moves.emplace_back(m_square, target);
                }
                else
                {
                    if (target->getPiece()->getOwner() != m_owner)
                    {
                        moves.emplace_back(m_square, target);
                    }
                    break;
                }
            }
        }
    }

    if (m_type == PieceType::Knight)
    {
        const int offsets[8][2] = {
            {-2, -1},
            {-2, 1},
            {2, -1},
            {2, 1},
            {-1, -2},
            {-1, 2},
            {1, -2},
            {1, 2}
        };

        for (const auto& offset : offsets)
        {
            const int nr = row + offset[0];
            const int nc = col + offset[1];

            if (m_board->isValid(nr, nc))
            {
                Square* target = m_board->getSquare(m_board->toIndex(nr, nc));

                if (!target->isOccupied() || target->getPiece()->getOwner() != m_owner)
                {
                    moves.emplace_back(m_square, target);
                }
            }
        }
    }

    if (m_type == PieceType::Queen)
    {
        const int directions[8][2] = {
            {-1, 0}, {1, 0}, {0, -1}, {0, 1},
            {-1, -1}, {-1, 1}, {1, -1}, {1, 1}
        };

        for (const auto& direction : directions)
        {
            int nr = row;
            int nc = col;

            while (true)
            {
                nr += direction[0];
                nc += direction[1];

                if (!m_board->isValid(nr, nc))
                {
                    break;
                }

                Square* target = m_board->getSquare(m_board->toIndex(nr, nc));

                if (!target->isOccupied())
                {
                    moves.emplace_back(m_square, target);
                }
                else
                {
                    if (target->getPiece()->getOwner() != m_owner)
                    {
                        moves.emplace_back(m_square, target);
                    }
                    break;
                }
            }
        }
    }

    if (m_type == PieceType::Pawn)
    {
        int direction = 1;

        if (m_owner->getColor() == PlayerColor::Red)
        {
            direction = -1;
        }
        else if (m_owner->getColor() == PlayerColor::Blue)
        {
            direction = 1;
        }
        else if (m_owner->getColor() == PlayerColor::Green)
        {
            direction = -1;
        }

        const int forwardRow = row + direction;
        const int forwardCol = col;

        if (m_board->isValid(forwardRow, forwardCol))
        {
            Square* forward = m_board->getSquare(m_board->toIndex(forwardRow, forwardCol));

            if (!forward->isOccupied())
            {
                moves.emplace_back(m_square, forward);
            }
        }

        for (int dc : {-1, 1})
        {
            const int captureRow = row + direction;
            const int captureCol = col + dc;

            if (m_board->isValid(captureRow, captureCol))
            {
                Square* target = m_board->getSquare(m_board->toIndex(captureRow, captureCol));

                if (target->isOccupied() && target->getPiece()->getOwner() != m_owner)
                {
                    moves.emplace_back(m_square, target);
                }
            }
        }
    }

    return moves;
}