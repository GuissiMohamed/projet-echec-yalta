#include "patterns/MoveCommand.hpp"

#include <sstream>

#include "core/Game.hpp"

MoveCommand::MoveCommand(Game& game, const Move& move)
    : m_game(game), m_move(move)
{
}

bool MoveCommand::execute()
{
    return m_game.playMove(m_move);
}

std::string MoveCommand::getLabel() const
{
    if (!m_move.isValid())
    {
        return "Commande de coup invalide";
    }

    std::ostringstream stream;
    stream << "MoveCommand : "
           << m_move.getFromId()
           << " -> "
           << m_move.getToId()
           << " ["
           << toString(m_move.getType())
           << "]";

    if (m_move.getType() == MoveType::Promotion)
    {
        stream << " => " << toString(m_move.getPromotionType());
    }

    return stream.str();
}