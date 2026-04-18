#pragma once

#include <optional>
#include <string>

#include "core/Move.hpp"
#include "core/Types.hpp"

class Game;
class ConsoleView;

struct StrategyDecision
{
    bool quitRequested = false;
    std::optional<Move> selectedMove;
};

class IMoveStrategy
{
public:
    virtual ~IMoveStrategy() = default;

    virtual StrategyDecision chooseMove(
        Game& game,
        ConsoleView& view,
        PlayerColor playerColor) = 0;

    virtual std::string getName() const = 0;
};