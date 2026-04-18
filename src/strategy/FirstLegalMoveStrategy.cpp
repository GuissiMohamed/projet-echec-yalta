#include "strategy/FirstLegalMoveStrategy.hpp"

#include "core/Game.hpp"
#include "mvc/ConsoleView.hpp"

StrategyDecision FirstLegalMoveStrategy::chooseMove(
    Game& game,
    ConsoleView& view,
    PlayerColor playerColor)
{
    const std::vector<Move> legalMoves = game.getLegalMovesForPlayer(playerColor);

    if (legalMoves.empty())
    {
        view.showMessage("FirstLegalMoveStrategy : aucun coup legal disponible.");
        return {};
    }

    view.showMessage("FirstLegalMoveStrategy : le premier coup legal est selectionne automatiquement.");

    StrategyDecision decision;
    decision.selectedMove = legalMoves.front();
    return decision;
}

std::string FirstLegalMoveStrategy::getName() const
{
    return "FirstLegalMoveStrategy";
}