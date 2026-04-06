#pragma once

#include <string>

enum class PlayerColor
{
    Red,
    Blue,
    Green,
    None
};

enum class PieceType
{
    King,
    Queen,
    Rook,
    Bishop,
    Knight,
    Pawn,
    None
};

inline std::string toString(PlayerColor color)
{
    switch (color)
    {
        case PlayerColor::Red: return "Red";
        case PlayerColor::Blue: return "Blue";
        case PlayerColor::Green: return "Green";
        default: return "None";
    }
}

inline std::string toString(PieceType type)
{
    switch (type)
    {
        case PieceType::King: return "King";
        case PieceType::Queen: return "Queen";
        case PieceType::Rook: return "Rook";
        case PieceType::Bishop: return "Bishop";
        case PieceType::Knight: return "Knight";
        case PieceType::Pawn: return "Pawn";
        default: return "None";
    }
}