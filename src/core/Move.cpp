#include "core/Move.hpp"

Move::Move(Square* from, Square* to)
    : m_from(from), m_to(to)
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