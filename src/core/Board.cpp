#include "core/Board.hpp"

#include "core/YaltaBoardTopology.hpp"

Board::Board()
    : m_topology(std::make_unique<YaltaBoardTopology>())
{
    initializeSquares();
}

Board::Board(std::unique_ptr<BoardTopology> topology)
    : m_topology(std::move(topology))
{
    if (!m_topology)
    {
        m_topology = std::make_unique<YaltaBoardTopology>();
    }

    initializeSquares();
}

void Board::initializeSquares()
{
    m_squares.clear();
    m_squares.reserve(m_topology->getSquareCount());

    for (int i = 0; i < m_topology->getSquareCount(); ++i)
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

    if (!m_topology->isActiveSquare(id))
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

    if (!m_topology->isActiveSquare(id))
    {
        return nullptr;
    }

    return &m_squares[id];
}

Square* Board::getSquare(int row, int col)
{
    const int id = toIndex(row, col);
    if (id < 0)
    {
        return nullptr;
    }

    return getSquare(id);
}

const Square* Board::getSquare(int row, int col) const
{
    const int id = toIndex(row, col);
    if (id < 0)
    {
        return nullptr;
    }

    return getSquare(id);
}

int Board::getSquareCount() const
{
    return m_topology->getSquareCount();
}

int Board::getRowCount() const
{
    return m_topology->getRowCount();
}

int Board::getColCount() const
{
    return m_topology->getColCount();
}

const std::vector<Square>& Board::getSquares() const
{
    return m_squares;
}

int Board::getRow(int id) const
{
    return m_topology->getRow(id);
}

int Board::getCol(int id) const
{
    return m_topology->getCol(id);
}

int Board::toIndex(int row, int col) const
{
    return m_topology->toIndex(row, col);
}

bool Board::isValid(int row, int col) const
{
    const int id = m_topology->toIndex(row, col);
    return id >= 0 && m_topology->isActiveSquare(id);
}

std::string Board::getLabel(int id) const
{
    return m_topology->getLabel(id);
}

bool Board::isActiveSquare(int id) const
{
    return m_topology->isActiveSquare(id);
}

float Board::getDisplayX(int id) const
{
    return m_topology->getDisplayX(id);
}

float Board::getDisplayY(int id) const
{
    return m_topology->getDisplayY(id);
}

int Board::getOrthogonalNeighbor(int squareId, OrthogonalDirection direction) const
{
    const int id = m_topology->getOrthogonalNeighbor(squareId, direction);
    return (id >= 0 && m_topology->isActiveSquare(id)) ? id : -1;
}

int Board::getDiagonalNeighbor(int squareId, DiagonalDirection direction) const
{
    const int id = m_topology->getDiagonalNeighbor(squareId, direction);
    return (id >= 0 && m_topology->isActiveSquare(id)) ? id : -1;
}

std::unique_ptr<BoardTopology> Board::cloneTopology() const
{
    return m_topology->clone();
}