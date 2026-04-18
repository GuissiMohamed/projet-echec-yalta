#pragma once

#include <memory>

#include "core/Game.hpp"

class ConsoleView;
class IMoveStrategy;

class GameController
{
public:
    GameController(
        Game& game,
        ConsoleView& view,
        std::unique_ptr<IMoveStrategy> redStrategy,
        std::unique_ptr<IMoveStrategy> blueStrategy,
        std::unique_ptr<IMoveStrategy> greenStrategy);

    void setupDefaultPosition();
    void run();

private:
    bool isSessionFinished() const;
    IMoveStrategy* getStrategy(PlayerColor color) const;

    void placeHorizontalArmy(PlayerColor color, int backRow, int pawnRow);
    void placeVerticalArmy(PlayerColor color, int backCol, int pawnCol);

    Game& m_game;
    ConsoleView& m_view;

    std::unique_ptr<IMoveStrategy> m_redStrategy;
    std::unique_ptr<IMoveStrategy> m_blueStrategy;
    std::unique_ptr<IMoveStrategy> m_greenStrategy;

    bool m_userRequestedQuit;
};