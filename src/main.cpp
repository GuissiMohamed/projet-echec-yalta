#include <iostream>
#include "core/Board.hpp"
#include "core/Player.hpp"
#include "core/Piece.hpp"
#include "core/Types.hpp"

int main()
{
    Board board(96);

    Player redPlayer("Alice", PlayerColor::Red);
    Player bluePlayer("Bob", PlayerColor::Blue);
    Player greenPlayer("Charlie", PlayerColor::Green);

    Square* startSquare = board.getSquare(0);

    Piece redKing(PieceType::King, &redPlayer, startSquare);
    redPlayer.addPiece(&redKing);

    if (startSquare != nullptr)
    {
        startSquare->setPiece(&redKing);
    }

    std::cout << "Board created with " << board.getSquareCount() << " squares.\n";
    std::cout << "Players: "
              << redPlayer.getName() << " (" << toString(redPlayer.getColor()) << "), "
              << bluePlayer.getName() << " (" << toString(bluePlayer.getColor()) << "), "
              << greenPlayer.getName() << " (" << toString(greenPlayer.getColor()) << ")\n";

    if (startSquare != nullptr && startSquare->isOccupied())
    {
        std::cout << "Square " << startSquare->getId()
                  << " contains a "
                  << toString(startSquare->getPiece()->getType())
                  << ".\n";
    }

    return 0;
}