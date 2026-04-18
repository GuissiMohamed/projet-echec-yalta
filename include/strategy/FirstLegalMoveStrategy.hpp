#pragma once

#include "strategy/IMoveStrategy.hpp"

class FirstLegalMoveStrategy : public IMoveStrategy
{
public:
    StrategyDecision chooseMove(
        Game& game,
        ConsoleView& view,
        PlayerColor playerColor) override;

    std::string getName() const override;
};