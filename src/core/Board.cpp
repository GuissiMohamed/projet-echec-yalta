#include "core/Board.hpp"

Board::Board(int squareCount)
{
    m_squares.reserve(squareCount);

    for (int i = 0; i < squareCount; ++i)
    {
        m_squares.emplace_back(i);
    }
}

Square* Board::getSquare(int id)
{
    if (id < 0 || id >= static_cast<int>(m_squares.size()))
    {
        return nullptr;
    }

    return &m_squares[id];
}

const Square* Board::getSquare(int id) const
{
    if (id < 0 || id >= static_cast<int>(m_squares.size()))
    {
        return nullptr;
    }

    return &m_squares[id];
}

int Board::getSquareCount() const
{
    return static_cast<int>(m_squares.size());
}

const std::vector<Square>& Board::getSquares() const
{
    return m_squares;
}