#include "core/Piece.hpp"

Piece::Piece(PieceType type, Player* owner, Square* square)
    : m_type(type), m_owner(owner), m_square(square)
{
}

PieceType Piece::getType() const
{
    return m_type;
}

Player* Piece::getOwner() const
{
    return m_owner;
}

Square* Piece::getSquare() const
{
    return m_square;
}

void Piece::setSquare(Square* square)
{
    m_square = square;
}