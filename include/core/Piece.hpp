#pragma once

#include "core/Types.hpp"

class Player;
class Square;

class Piece
{
public:
    Piece(PieceType type, Player* owner, Square* square);
    virtual ~Piece() = default;

    PieceType getType() const;
    Player* getOwner() const;
    Square* getSquare() const;

    void setSquare(Square* square);

protected:
    PieceType m_type;
    Player* m_owner;
    Square* m_square;
};