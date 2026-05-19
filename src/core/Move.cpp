#include "core/Move.hpp"

#include "core/Board.hpp"
#include "core/Square.hpp"

Move::Move(int fromId, int toId, MoveType type, PieceType promotionType)
    : m_fromId(fromId),
      m_toId(toId),
      m_type(type),
      m_promotionType(promotionType)
{
}

int Move::getFromId() const
{
    return m_fromId;
}

int Move::getToId() const
{
    return m_toId;
}

MoveType Move::getType() const
{
    return m_type;
}

PieceType Move::getPromotionType() const
{
    return m_promotionType;
}

bool Move::isValid() const
{
    return m_fromId >= 0 && m_toId >= 0;
}

Square* Move::resolveFrom(Board& board) const
{
    return board.getSquare(m_fromId);
}

const Square* Move::resolveFrom(const Board& board) const
{
    return board.getSquare(m_fromId);
}

Square* Move::resolveTo(Board& board) const
{
    return board.getSquare(m_toId);
}

const Square* Move::resolveTo(const Board& board) const
{
    return board.getSquare(m_toId);
}

bool Move::operator==(const Move& other) const
{
    return m_fromId == other.m_fromId
        && m_toId == other.m_toId
        && m_type == other.m_type
        && m_promotionType == other.m_promotionType;
}