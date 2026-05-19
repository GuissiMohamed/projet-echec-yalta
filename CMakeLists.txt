cmake_minimum_required(VERSION 3.25)
project(projet-echec-yalta LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SFML 3 REQUIRED COMPONENTS Graphics)

include_directories(include)

add_executable(projet-echec-yalta
    src/main.cpp
    src/core/Square.cpp
    src/core/Board.cpp
    src/core/BoardTopology.cpp
    src/core/YaltaBoardTopology.cpp
    src/core/Player.cpp
    src/core/Move.cpp
    src/core/Piece.cpp
    src/core/Game.cpp
    src/mvc/ConsoleView.cpp
    src/mvc/GameController.cpp
    src/mvc/SFMLView.cpp
    src/mvc/SFMLGameController.cpp
    src/patterns/MoveCommand.cpp
    src/strategy/HumanConsoleStrategy.cpp
    src/strategy/FirstLegalMoveStrategy.cpp
    src/ai/AlphaBetaAI.cpp
)

target_link_libraries(projet-echec-yalta PRIVATE SFML::Graphics)

add_executable(yalta-core-tests
    tests/core_tests.cpp
    src/core/Square.cpp
    src/core/Board.cpp
    src/core/BoardTopology.cpp
    src/core/YaltaBoardTopology.cpp
    src/core/Player.cpp
    src/core/Move.cpp
    src/core/Piece.cpp
    src/core/Game.cpp
)

enable_testing()
add_test(NAME yalta-core-tests COMMAND yalta-core-tests)