#include <iostream>
#include <vector>

#include "core/Board.hpp"
#include "core/Move.hpp"
#include "core/Piece.hpp"
#include "core/Player.hpp"
#include "core/Types.hpp"

void printMoves(const std::string& pieceName, const std::vector<Move>& moves)
{
    std::cout << pieceName << " has " << moves.size() << " possible moves:\n";

    for (const Move& move : moves)
    {
        if (move.getFrom() != nullptr && move.getTo() != nullptr)
        {
            std::cout << "  " << move.getFrom()->getId()
                      << " -> " << move.getTo()->getId() << "\n";
        }
    }

    std::cout << '\n';
}

int main()
{
    Board board(96);

    Player redPlayer("Alice", PlayerColor::Red);
    Player bluePlayer("Bob", PlayerColor::Blue);
    Player greenPlayer("Charlie", PlayerColor::Green);

    Square* kingSquare = board.getSquare(0);
    Piece redKing(PieceType::King, &redPlayer, kingSquare, &board);
    redPlayer.addPiece(&redKing);
    if (kingSquare != nullptr)
    {
        kingSquare->setPiece(&redKing);
    }

    Square* rookSquare = board.getSquare(10);
    Piece redRook(PieceType::Rook, &redPlayer, rookSquare, &board);
    redPlayer.addPiece(&redRook);
    if (rookSquare != nullptr)
    {
        rookSquare->setPiece(&redRook);
    }

    Square* bishopSquare = board.getSquare(40);
    Piece blueBishop(PieceType::Bishop, &bluePlayer, bishopSquare, &board);
    bluePlayer.addPiece(&blueBishop);
    if (bishopSquare != nullptr)
    {
        bishopSquare->setPiece(&blueBishop);
    }

    Square* knightSquare = board.getSquare(50);
    Piece greenKnight(PieceType::Knight, &greenPlayer, knightSquare, &board);
    greenPlayer.addPiece(&greenKnight);
    if (knightSquare != nullptr)
    {
        knightSquare->setPiece(&greenKnight);
    }

    std::cout << "Board created with " << board.getSquareCount() << " squares.\n";
    std::cout << "Players: "
              << redPlayer.getName() << " (" << toString(redPlayer.getColor()) << "), "
              << bluePlayer.getName() << " (" << toString(bluePlayer.getColor()) << "), "
              << greenPlayer.getName() << " (" << toString(greenPlayer.getColor()) << ")\n\n";

    printMoves("King", redKing.getLegalMoves());
    printMoves("Rook", redRook.getLegalMoves());
    printMoves("Bishop", blueBishop.getLegalMoves());
    printMoves("Knight", greenKnight.getLegalMoves());

    return 0;
}