#pragma once

class Piece;

class Square
{
public:
    explicit Square(int id = -1);

    int getId() const;

    bool isOccupied() const;
    Piece* getPiece() const;

    void setPiece(Piece* piece);
    void removePiece();

private:
    int m_id;
    Piece* m_piece;
};