#pragma once

#include "core/Types.hpp"

class Board;
class Square;

class Move
{
public:
    Move(
        int fromId = -1,
        int toId = -1,
        MoveType type = MoveType::Normal,
        PieceType promotionType = PieceType::None
    );

    int getFromId() const;
    int getToId() const;
    MoveType getType() const;
    PieceType getPromotionType() const;

    bool isValid() const;

    Square* resolveFrom(Board& board) const;
    const Square* resolveFrom(const Board& board) const;

    Square* resolveTo(Board& board) const;
    const Square* resolveTo(const Board& board) const;

    bool operator==(const Move& other) const;

private:
    int m_fromId;
    int m_toId;
    MoveType m_type;
    PieceType m_promotionType;
};