#pragma once

#include <string>
#include <utility>
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
    void clearPieces();
    const std::vector<Piece*>& getPieces() const;

    bool isEliminated() const;
    void setEliminated(bool eliminated);

    std::pair<int, int> getForwardDirection() const;
    std::vector<std::pair<int, int>> getPawnCaptureDirections() const;

private:
    std::string m_name;
    PlayerColor m_color;
    std::vector<Piece*> m_pieces;
    bool m_eliminated;
};