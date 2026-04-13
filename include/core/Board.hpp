#pragma once

#include <vector>
#include "core/Square.hpp"

class Board
{
public:
    static constexpr int kRows = 8;
    static constexpr int kCols = 12;
    static constexpr int kSquareCount = kRows * kCols;

    explicit Board(int squareCount = kSquareCount);

    Square* getSquare(int id);
    const Square* getSquare(int id) const;

    Square* getSquare(int row, int col);
    const Square* getSquare(int row, int col) const;

    int getSquareCount() const;
    int getRowCount() const;
    int getColCount() const;
    const std::vector<Square>& getSquares() const;

    int getRow(int id) const;
    int getCol(int id) const;
    int toIndex(int row, int col) const;
    bool isValid(int row, int col) const;

private:
    std::vector<Square> m_squares;
};