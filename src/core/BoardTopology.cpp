#include "core/BoardTopology.hpp"

#include <memory>
#include <sstream>

RectangularBoardTopology::RectangularBoardTopology(int rows, int cols)
    : m_rows(rows),
      m_cols(cols)
{
    m_squares.reserve(rows * cols);

    for (int row = 0; row < rows; ++row)
    {
        for (int col = 0; col < cols; ++col)
        {
            const int id = (row * cols) + col;

            std::ostringstream labelStream;
            labelStream << static_cast<char>('a' + col) << (rows - row);

            TopologySquare square;
            square.id = id;
            square.logicalRow = row;
            square.logicalCol = col;
            square.displayX = static_cast<float>(col);
            square.displayY = static_cast<float>(row);
            square.active = true;
            square.label = labelStream.str();

            m_squares.push_back(square);
        }
    }
}

int RectangularBoardTopology::getSquareCount() const
{
    return static_cast<int>(m_squares.size());
}

int RectangularBoardTopology::getRowCount() const
{
    return m_rows;
}

int RectangularBoardTopology::getColCount() const
{
    return m_cols;
}

bool RectangularBoardTopology::isValidCoordinate(int row, int col) const
{
    return row >= 0 && row < m_rows && col >= 0 && col < m_cols;
}

int RectangularBoardTopology::toIndex(int row, int col) const
{
    if (!isValidCoordinate(row, col))
    {
        return -1;
    }

    return (row * m_cols) + col;
}

int RectangularBoardTopology::getRow(int id) const
{
    if (id < 0 || id >= getSquareCount())
    {
        return -1;
    }

    return m_squares[id].logicalRow;
}

int RectangularBoardTopology::getCol(int id) const
{
    if (id < 0 || id >= getSquareCount())
    {
        return -1;
    }

    return m_squares[id].logicalCol;
}

std::string RectangularBoardTopology::getLabel(int id) const
{
    if (id < 0 || id >= getSquareCount())
    {
        return "invalid";
    }

    return m_squares[id].label;
}

bool RectangularBoardTopology::isActiveSquare(int id) const
{
    if (id < 0 || id >= getSquareCount())
    {
        return false;
    }

    return m_squares[id].active;
}

float RectangularBoardTopology::getDisplayX(int id) const
{
    if (id < 0 || id >= getSquareCount())
    {
        return -1.0f;
    }

    return m_squares[id].displayX;
}

float RectangularBoardTopology::getDisplayY(int id) const
{
    if (id < 0 || id >= getSquareCount())
    {
        return -1.0f;
    }

    return m_squares[id].displayY;
}

const std::vector<TopologySquare>& RectangularBoardTopology::getSquareDefinitions() const
{
    return m_squares;
}

std::unique_ptr<BoardTopology> RectangularBoardTopology::clone() const
{
    return std::make_unique<RectangularBoardTopology>(*this);
}

int RectangularBoardTopology::getOrthogonalNeighbor(int squareId, OrthogonalDirection direction) const
{
    if (squareId < 0 || squareId >= getSquareCount())
    {
        return -1;
    }

    const int row = getRow(squareId);
    const int col = getCol(squareId);

    int nextRow = row;
    int nextCol = col;

    switch (direction)
    {
        case OrthogonalDirection::Up:    --nextRow; break;
        case OrthogonalDirection::Down:  ++nextRow; break;
        case OrthogonalDirection::Left:  --nextCol; break;
        case OrthogonalDirection::Right: ++nextCol; break;
    }

    return toIndex(nextRow, nextCol);
}

int RectangularBoardTopology::getDiagonalNeighbor(int squareId, DiagonalDirection direction) const
{
    if (squareId < 0 || squareId >= getSquareCount())
    {
        return -1;
    }

    const int row = getRow(squareId);
    const int col = getCol(squareId);

    int nextRow = row;
    int nextCol = col;

    switch (direction)
    {
        case DiagonalDirection::UpLeft:
            --nextRow;
            --nextCol;
            break;
        case DiagonalDirection::UpRight:
            --nextRow;
            ++nextCol;
            break;
        case DiagonalDirection::DownLeft:
            ++nextRow;
            --nextCol;
            break;
        case DiagonalDirection::DownRight:
            ++nextRow;
            ++nextCol;
            break;
    }

    return toIndex(nextRow, nextCol);
}