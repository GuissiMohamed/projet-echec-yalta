#include "core/YaltaBoardTopology.hpp"

#include <cmath>
#include <memory>
#include <sstream>

YaltaBoardTopology::YaltaBoardTopology()
{
    m_squares.reserve(kRows * kCols);

    for (int row = 0; row < kRows; ++row)
    {
        for (int col = 0; col < kCols; ++col)
        {
            const int id = (row * kCols) + col;

            std::ostringstream labelStream;
            labelStream << static_cast<char>('a' + col) << (kRows - row);

            const float centerRow = 3.5f;
            const float distanceFromCenter = static_cast<float>(row) - centerRow;

            float rowShift = distanceFromCenter * 0.75f;

            float sectorCurve = 0.0f;
            if (col <= 3)
            {
                sectorCurve = -0.18f * static_cast<float>(4 - col);
            }
            else if (col >= 8)
            {
                sectorCurve = 0.18f * static_cast<float>(col - 7);
            }

            float verticalBend = 0.0f;
            if (col <= 3 || col >= 8)
            {
                verticalBend = std::abs(distanceFromCenter) * 0.08f;
            }

            TopologySquare square;
            square.id = id;
            square.logicalRow = row;
            square.logicalCol = col;
            square.displayX = static_cast<float>(col) + rowShift + sectorCurve;
            square.displayY = static_cast<float>(row) * 0.9f + verticalBend;
            square.active = true;
            square.label = labelStream.str();

            m_squares.push_back(square);
        }
    }
}

int YaltaBoardTopology::getSquareCount() const
{
    return static_cast<int>(m_squares.size());
}

int YaltaBoardTopology::getRowCount() const
{
    return kRows;
}

int YaltaBoardTopology::getColCount() const
{
    return kCols;
}

bool YaltaBoardTopology::isValidCoordinate(int row, int col) const
{
    return row >= 0 && row < kRows && col >= 0 && col < kCols;
}

int YaltaBoardTopology::toIndex(int row, int col) const
{
    if (!isValidCoordinate(row, col))
    {
        return -1;
    }

    return (row * kCols) + col;
}

int YaltaBoardTopology::getRow(int id) const
{
    if (id < 0 || id >= getSquareCount())
    {
        return -1;
    }

    return m_squares[id].logicalRow;
}

int YaltaBoardTopology::getCol(int id) const
{
    if (id < 0 || id >= getSquareCount())
    {
        return -1;
    }

    return m_squares[id].logicalCol;
}

std::string YaltaBoardTopology::getLabel(int id) const
{
    if (id < 0 || id >= getSquareCount())
    {
        return "invalid";
    }

    return m_squares[id].label;
}

bool YaltaBoardTopology::isActiveSquare(int id) const
{
    if (id < 0 || id >= getSquareCount())
    {
        return false;
    }

    return m_squares[id].active;
}

float YaltaBoardTopology::getDisplayX(int id) const
{
    if (id < 0 || id >= getSquareCount())
    {
        return -1.0f;
    }

    return m_squares[id].displayX;
}

float YaltaBoardTopology::getDisplayY(int id) const
{
    if (id < 0 || id >= getSquareCount())
    {
        return -1.0f;
    }

    return m_squares[id].displayY;
}

const std::vector<TopologySquare>& YaltaBoardTopology::getSquareDefinitions() const
{
    return m_squares;
}

std::unique_ptr<BoardTopology> YaltaBoardTopology::clone() const
{
    return std::make_unique<YaltaBoardTopology>(*this);
}

int YaltaBoardTopology::getOrthogonalNeighbor(int squareId, OrthogonalDirection direction) const
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

int YaltaBoardTopology::getDiagonalNeighbor(int squareId, DiagonalDirection direction) const
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