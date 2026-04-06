#pragma once

#include <vector>
#include "core/Move.hpp"
#include "core/Types.hpp"

class Player;
class Square;
class Board;

class Piece
{
public:
    Piece(PieceType type, Player* owner, Square* square, Board* board);
    virtual ~Piece() = default;

    PieceType getType() const;
    Player* getOwner() const;
    Square* getSquare() const;
    Board* getBoard() const;

    void setSquare(Square* square);

    std::vector<Move> getLegalMoves() const;

private:
    PieceType m_type;
    Player* m_owner;
    Square* m_square;
    Board* m_board;
};