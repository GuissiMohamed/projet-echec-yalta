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

Square* Board::getSquare(int row, int col)
{
    if (!isValid(row, col))
    {
        return nullptr;
    }

    return getSquare(toIndex(row, col));
}

const Square* Board::getSquare(int row, int col) const
{
    if (!isValid(row, col))
    {
        return nullptr;
    }

    return getSquare(toIndex(row, col));
}

int Board::getSquareCount() const
{
    return static_cast<int>(m_squares.size());
}

int Board::getRowCount() const
{
    return kRows;
}

int Board::getColCount() const
{
    return kCols;
}

const std::vector<Square>& Board::getSquares() const
{
    return m_squares;
}

int Board::getRow(int id) const
{
    return id / kCols;
}

int Board::getCol(int id) const
{
    return id % kCols;
}

int Board::toIndex(int row, int col) const
{
    return row * kCols + col;
}

bool Board::isValid(int row, int col) const
{
    return row >= 0 && row < kRows && col >= 0 && col < kCols;
}