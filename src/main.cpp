#include <memory>

#include "core/Game.hpp"
#include "mvc/ConsoleView.hpp"
#include "mvc/GameController.hpp"
#include "strategy/FirstLegalMoveStrategy.hpp"
#include "strategy/HumanConsoleStrategy.hpp"
#include "strategy/IMoveStrategy.hpp"

int main()
{
    Game game;
    ConsoleView view;

    std::unique_ptr<IMoveStrategy> redStrategy = std::make_unique<HumanConsoleStrategy>();
    std::unique_ptr<IMoveStrategy> blueStrategy = std::make_unique<HumanConsoleStrategy>();
    std::unique_ptr<IMoveStrategy> greenStrategy = std::make_unique<HumanConsoleStrategy>();

    // Variante de demonstration du pattern Strategy :
    // std::unique_ptr<IMoveStrategy> blueStrategy = std::make_unique<FirstLegalMoveStrategy>();
    // std::unique_ptr<IMoveStrategy> greenStrategy = std::make_unique<FirstLegalMoveStrategy>();

    GameController controller(
        game,
        view,
        std::move(redStrategy),
        std::move(blueStrategy),
        std::move(greenStrategy));

    controller.run();
    return 0;
}