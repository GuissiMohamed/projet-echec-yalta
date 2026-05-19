#pragma once

#include <memory>
#include <string>
#include <vector>

#include "core/BoardTopology.hpp"
#include "core/Square.hpp"

class Board
{
public:
    static constexpr int kRows = 8;
    static constexpr int kCols = 12;
    static constexpr int kSquareCount = kRows * kCols;

    Board();
    explicit Board(std::unique_ptr<BoardTopology> topology);

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

    std::string getLabel(int id) const;
    bool isActiveSquare(int id) const;
    float getDisplayX(int id) const;
    float getDisplayY(int id) const;

    int getOrthogonalNeighbor(int squareId, OrthogonalDirection direction) const;
    int getDiagonalNeighbor(int squareId, DiagonalDirection direction) const;

    std::unique_ptr<BoardTopology> cloneTopology() const;

private:
    void initializeSquares();

    std::unique_ptr<BoardTopology> m_topology;
    std::vector<Square> m_squares;
};