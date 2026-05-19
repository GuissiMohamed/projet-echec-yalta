#pragma once

#include <array>
#include <memory>
#include <string>
#include <vector>

#include "core/TopologySquare.hpp"

enum class OrthogonalDirection
{
    Up = 0,
    Down = 1,
    Left = 2,
    Right = 3
};

enum class DiagonalDirection
{
    UpLeft = 0,
    UpRight = 1,
    DownLeft = 2,
    DownRight = 3
};

class BoardTopology
{
public:
    virtual ~BoardTopology() = default;

    virtual int getSquareCount() const = 0;
    virtual int getRowCount() const = 0;
    virtual int getColCount() const = 0;

    virtual bool isValidCoordinate(int row, int col) const = 0;
    virtual int toIndex(int row, int col) const = 0;
    virtual int getRow(int id) const = 0;
    virtual int getCol(int id) const = 0;
    virtual std::string getLabel(int id) const = 0;

    virtual bool isActiveSquare(int id) const = 0;
    virtual float getDisplayX(int id) const = 0;
    virtual float getDisplayY(int id) const = 0;

    virtual const std::vector<TopologySquare>& getSquareDefinitions() const = 0;
    virtual std::unique_ptr<BoardTopology> clone() const = 0;

    virtual int getOrthogonalNeighbor(int squareId, OrthogonalDirection direction) const = 0;
    virtual int getDiagonalNeighbor(int squareId, DiagonalDirection direction) const = 0;
};

class RectangularBoardTopology final : public BoardTopology
{
public:
    RectangularBoardTopology(int rows, int cols);

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
    int m_rows;
    int m_cols;
    std::vector<TopologySquare> m_squares;
};