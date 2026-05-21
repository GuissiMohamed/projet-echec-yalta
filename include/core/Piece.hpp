/**
 * @class Piece
 * @brief Représente une pièce du jeu
 *
 * Une pièce possède :
 * - un type (roi, reine, etc.)
 * - un joueur propriétaire
 * - une position
 */

#pragma once

#include <vector>
#include "core/Move.hpp"
#include "core/Types.hpp"

class Player;
class Square;
class Board;

class Piece
{
public:
    Piece(PieceType type, Player* owner, Square* square, Board* board);
    virtual ~Piece() = default;

    PieceType getType() const;
    void setType(PieceType type);

    Player* getOwner() const;
    Square* getSquare() const;
    Board* getBoard() const;

    void setSquare(Square* square);

    bool hasMoved() const;
    void setHasMoved(bool hasMoved);

    bool isCaptured() const;
    void setCaptured(bool captured);

    std::vector<Move> getPseudoLegalMoves() const;

private:
    void addSlidingMoves(std::vector<Move>& moves, const int directions[][2], int directionCount) const;

    PieceType m_type;
    Player* m_owner;
    Square* m_square;
    Board* m_board;
    bool m_hasMoved;
    bool m_isCaptured;
};