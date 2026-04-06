#include "core/Player.hpp"

Player::Player(const std::string& name, PlayerColor color)
    : m_name(name), m_color(color)
{
}

const std::string& Player::getName() const
{
    return m_name;
}

PlayerColor Player::getColor() const
{
    return m_color;
}

void Player::addPiece(Piece* piece)
{
    m_pieces.push_back(piece);
}

const std::vector<Piece*>& Player::getPieces() const
{
    return m_pieces;
}