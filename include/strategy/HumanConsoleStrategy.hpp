/**
 * @class HumanConsoleStrategy
 * @brief Stratégie de jeu pour un joueur humain
 *
 * Permet de récupérer un coup depuis la console.
 */

#pragma once

#include "strategy/IMoveStrategy.hpp"

class HumanConsoleStrategy : public IMoveStrategy
{
public:
    StrategyDecision chooseMove(
        Game& game,
        ConsoleView& view,
        PlayerColor playerColor) override;

    std::string getName() const override;
};