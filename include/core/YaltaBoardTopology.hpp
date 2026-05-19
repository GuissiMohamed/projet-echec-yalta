#pragma once

#include "core/BoardTopology.hpp"

class YaltaBoardTopology final : public BoardTopology
{
public:
    YaltaBoardTopology();

    int getSquareCount() const override;
    int getRowCount() const override;
    int getColCount() const override;

    bool isValidCoordinate(int row, int col) const override;
    int toIndex(int row, int col) const override;
    int getRow(int id) const override;
    int getCol(int id) const override;
    std::string getLabel(int id) const override;

    bool isActiveSquare(int id) const override;
    float getDisplayX(int id) const override;
    float getDisplayY(int id) const override;

    const std::vector<TopologySquare>& getSquareDefinitions() const override;
    std::unique_ptr<BoardTopology> clone() const override;

    int getOrthogonalNeighbor(int squareId, OrthogonalDirection direction) const override;
    int getDiagonalNeighbor(int squareId, DiagonalDirection direction) const override;

private:
    static constexpr int kRows = 8;
    static constexpr int kCols = 12;

    std::vector<TopologySquare> m_squares;
};