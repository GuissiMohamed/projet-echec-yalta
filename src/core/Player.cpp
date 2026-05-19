#include "core/Player.hpp"

Player::Player(const std::string& name, PlayerColor color)
    : m_name(name), m_color(color), m_eliminated(false)
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

void Player::clearPieces()
{
    m_pieces.clear();
}

const std::vector<Piece*>& Player::getPieces() const
{
    return m_pieces;
}

bool Player::isEliminated() const
{
    return m_eliminated;
}

void Player::setEliminated(bool eliminated)
{
    m_eliminated = eliminated;
}

OrthogonalDirection Player::getPawnForwardDirection() const
{
    switch (m_color)
    {
        case PlayerColor::Red:
            return OrthogonalDirection::Up;
        case PlayerColor::Blue:
            return OrthogonalDirection::Down;
        case PlayerColor::Green:
            return OrthogonalDirection::Left;
        default:
            return OrthogonalDirection::Up;
    }
}

std::pair<DiagonalDirection, DiagonalDirection> Player::getPawnCaptureDirections() const
{
    switch (m_color)
    {
        case PlayerColor::Red:
            return {DiagonalDirection::UpLeft, DiagonalDirection::UpRight};
        case PlayerColor::Blue:
            return {DiagonalDirection::DownLeft, DiagonalDirection::DownRight};
        case PlayerColor::Green:
            return {DiagonalDirection::UpLeft, DiagonalDirection::DownLeft};
        default:
            return {DiagonalDirection::UpLeft, DiagonalDirection::UpRight};
    }
}