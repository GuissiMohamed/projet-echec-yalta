/**
 * @class Player
 * @brief Représente un joueur
 *
 * Un joueur possède :
 * - un nom
 * - une couleur
 * - une liste de pièces
 */


#pragma once

#include <string>
#include <utility>
#include <vector>

#include "core/BoardTopology.hpp"
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

    OrthogonalDirection getPawnForwardDirection() const;
    std::pair<DiagonalDirection, DiagonalDirection> getPawnCaptureDirections() const;

private:
    std::string m_name;
    PlayerColor m_color;
    std::vector<Piece*> m_pieces;
    bool m_eliminated;
};