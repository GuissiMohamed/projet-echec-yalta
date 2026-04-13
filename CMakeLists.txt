
cmake_minimum_required(VERSION 3.25)
project(projet-echec-yalta LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)

add_executable(projet-echec-yalta
    src/main.cpp
    src/core/Square.cpp
    src/core/Board.cpp
    src/core/Player.cpp
    src/core/Move.cpp
    src/core/Piece.cpp
    src/core/Game.cpp
)

enable_testing()