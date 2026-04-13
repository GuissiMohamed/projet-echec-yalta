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

std::pair<int, int> Player::getForwardDirection() const
{
    switch (m_color)
    {
        case PlayerColor::Red:
            return {-1, 0};
        case PlayerColor::Blue:
            return {1, 0};
        case PlayerColor::Green:
            return {0, -1};
        default:
            return {0, 0};
    }
}

std::vector<std::pair<int, int>> Player::getPawnCaptureDirections() const
{
    const auto [forwardRow, forwardCol] = getForwardDirection();

    if (forwardRow != 0)
    {
        return {
            {forwardRow, -1},
            {forwardRow, 1}
        };
    }

    if (forwardCol != 0)
    {
        return {
            {-1, forwardCol},
            {1, forwardCol}
        };
    }

    return {};
}