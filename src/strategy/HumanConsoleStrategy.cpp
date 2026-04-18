#include "strategy/HumanConsoleStrategy.hpp"

#include <vector>

#include "core/Game.hpp"
#include "core/Square.hpp"
#include "mvc/ConsoleView.hpp"

namespace
{
    std::vector<Move> filterMovesByFrom(const std::vector<Move>& moves, int fromId)
    {
        std::vector<Move> result;

        for (const Move& move : moves)
        {
            if (move.getFrom() != nullptr && move.getFrom()->getId() == fromId)
            {
                result.push_back(move);
            }
        }

        return result;
    }

    std::vector<Move> filterMovesByTo(const std::vector<Move>& moves, int toId)
    {
        std::vector<Move> result;

        for (const Move& move : moves)
        {
            if (move.getTo() != nullptr && move.getTo()->getId() == toId)
            {
                result.push_back(move);
            }
        }

        return result;
    }

    Move selectPromotionMove(ConsoleView& view, const std::vector<Move>& promotionMoves)
    {
        const int choice = view.askPromotionChoice();

        PieceType selectedType = PieceType::Queen;

        switch (choice)
        {
            case 1: selectedType = PieceType::Queen; break;
            case 2: selectedType = PieceType::Rook; break;
            case 3: selectedType = PieceType::Bishop; break;
            case 4: selectedType = PieceType::Knight; break;
            default: selectedType = PieceType::Queen; break;
        }

        for (const Move& move : promotionMoves)
        {
            if (move.getPromotionType() == selectedType)
            {
                return move;
            }
        }

        return {};
    }
}

StrategyDecision HumanConsoleStrategy::chooseMove(
    Game& game,
    ConsoleView& view,
    PlayerColor playerColor)
{
    const std::vector<Move> playerMoves = game.getLegalMovesForPlayer(playerColor);

    if (playerMoves.empty())
    {
        view.showMessage("Aucun coup legal disponible pour cette strategie humaine.");
        return {};
    }

    while (true)
    {
        const std::string command = view.askCommand();

        if (command == "quit")
        {
            StrategyDecision decision;
            decision.quitRequested = true;
            return decision;
        }

        if (command != "play")
        {
            view.showMessage("Commande inconnue.");
            continue;
        }

        const int fromId = view.askSquareId("Case source : ");
        const std::vector<Move> pieceMoves = filterMovesByFrom(playerMoves, fromId);

        if (pieceMoves.empty())
        {
            view.showMessage("Aucun coup legal depuis cette case.");
            continue;
        }

        view.showMoves(pieceMoves);

        const int toId = view.askSquareId("Case destination : ");
        const std::vector<Move> destinationMoves = filterMovesByTo(pieceMoves, toId);

        if (destinationMoves.empty())
        {
            view.showMessage("Destination invalide pour cette piece.");
            continue;
        }

        Move selectedMove = destinationMoves.front();

        if (destinationMoves.size() > 1
            && destinationMoves.front().getType() == MoveType::Promotion)
        {
            selectedMove = selectPromotionMove(view, destinationMoves);
            if (selectedMove.getFrom() == nullptr)
            {
                view.showMessage("Choix de promotion invalide.");
                continue;
            }
        }

        StrategyDecision decision;
        decision.selectedMove = selectedMove;
        return decision;
    }
}

std::string HumanConsoleStrategy::getName() const
{
    return "HumanConsoleStrategy";
}