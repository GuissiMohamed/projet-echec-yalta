#include "core/Square.hpp"
#include "core/Piece.hpp"

Square::Square(int id)
    : m_id(id), m_piece(nullptr)
{
}

int Square::getId() const
{
    return m_id;
}

bool Square::isOccupied() const
{
    return m_piece != nullptr;
}

Piece* Square::getPiece() const
{
    return m_piece;
}

void Square::setPiece(Piece* piece)
{
    m_piece = piece;
}

void Square::removePiece()
{
    m_piece = nullptr;
}