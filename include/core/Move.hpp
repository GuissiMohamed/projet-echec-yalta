#pragma once

#include "core/Types.hpp"

class Square;

class Move
{
public:
    Move(
        Square* from = nullptr,
        Square* to = nullptr,
        MoveType type = MoveType::Normal,
        PieceType promotionType = PieceType::None
    );

    Square* getFrom() const;
    Square* getTo() const;
    MoveType getType() const;
    PieceType getPromotionType() const;

    bool operator==(const Move& other) const;

private:
    Square* m_from;
    Square* m_to;
    MoveType m_type;
    PieceType m_promotionType;
};