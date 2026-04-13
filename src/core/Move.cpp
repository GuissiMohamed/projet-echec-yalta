#include "core/Move.hpp"
#include "core/Square.hpp"

Move::Move(Square* from, Square* to, MoveType type, PieceType promotionType)
    : m_from(from), m_to(to), m_type(type), m_promotionType(promotionType)
{
}

Square* Move::getFrom() const
{
    return m_from;
}

Square* Move::getTo() const
{
    return m_to;
}

MoveType Move::getType() const
{
    return m_type;
}

PieceType Move::getPromotionType() const
{
    return m_promotionType;
}

bool Move::operator==(const Move& other) const
{
    const int thisFromId = (m_from != nullptr) ? m_from->getId() : -1;
    const int thisToId = (m_to != nullptr) ? m_to->getId() : -1;
    const int otherFromId = (other.m_from != nullptr) ? other.m_from->getId() : -1;
    const int otherToId = (other.m_to != nullptr) ? other.m_to->getId() : -1;

    return thisFromId == otherFromId
        && thisToId == otherToId
        && m_type == other.m_type
        && m_promotionType == other.m_promotionType;
}