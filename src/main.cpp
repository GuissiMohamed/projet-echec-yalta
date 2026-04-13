#include "core/Game.hpp"
#include "mvc/ConsoleView.hpp"
#include "mvc/GameController.hpp"

int main()
{
    Game game;
    ConsoleView view;
    GameController controller(game, view);

    controller.run();
    return 0;
}