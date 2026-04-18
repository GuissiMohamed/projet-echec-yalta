#include "core/Game.hpp"
#include "mvc/SFMLGameController.hpp"
#include "mvc/SFMLView.hpp"

int main()
{
    Game game;
    SFMLView view;
    SFMLGameController controller(game, view);

    controller.run();
    return 0;
}