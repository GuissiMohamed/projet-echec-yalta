#pragma once

#include <vector>
#include "core/Square.hpp"

class Board
{
public:
    explicit Board(int squareCount = 96);

    Square* getSquare(int id);
    const Square* getSquare(int id) const;

    int getSquareCount() const;
    const std::vector<Square>& getSquares() const;

    int getRow(int id) const;
    int getCol(int id) const;
    int toIndex(int row, int col) const;
    bool isValid(int row, int col) const;

private:
    std::vector<Square> m_squares;
};