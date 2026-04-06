#pragma once

#include <string>
#include <vector>
#include "core/Types.hpp"

class Piece;

class Player
{
public:
    Player(const std::string& name, PlayerColor color);

    const std::string& getName() const;
    PlayerColor getColor() const;

    void addPiece(Piece* piece);
    const std::vector<Piece*>& getPieces() const;

private:
    std::string m_name;
    PlayerColor m_color;
    std::vector<Piece*> m_pieces;
};