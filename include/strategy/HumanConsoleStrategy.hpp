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