#include <iostream>
#include <string>
#include <vector>

#include "core/Game.hpp"
#include "core/Piece.hpp"
#include "core/Square.hpp"

namespace
{
    std::string moveToString(const Move& move)
    {
        if (move.getFrom() == nullptr || move.getTo() == nullptr)
        {
            return "invalid move";
        }

        std::string label = std::to_string(move.getFrom()->getId())
            + " -> "
            + std::to_string(move.getTo()->getId())
            + " ["
            + toString(move.getType())
            + "]";

        if (move.getType() == MoveType::Promotion)
        {
            label += " => " + toString(move.getPromotionType());
        }

        return label;
    }

    void printMoveList(const std::string& title, const std::vector<Move>& moves)
    {
        std::cout << title << " (" << moves.size() << " coups)\n";
        for (const Move& move : moves)
        {
            std::cout << "  - " << moveToString(move) << '\n';
        }
        std::cout << '\n';
    }

    Move findMove(
        const std::vector<Move>& moves,
        int fromId,
        int toId,
        MoveType type,
        PieceType promotionType = PieceType::None)
    {
        for (const Move& move : moves)
        {
            if (move.getFrom() != nullptr
                && move.getTo() != nullptr
                && move.getFrom()->getId() == fromId
                && move.getTo()->getId() == toId
                && move.getType() == type
                && move.getPromotionType() == promotionType)
            {
                return move;
            }
        }

        return {};
    }

    void printHeader(const std::string& title)
    {
        std::cout << "\n============================================================\n";
        std::cout << title << '\n';
        std::cout << "============================================================\n";
    }

    void scenarioCastling()
    {
        printHeader("SCENARIO 1 - Roque (implementation temporaire sur la grille 8x12)");

        Game game;
        game.addPiece(PieceType::King, PlayerColor::Red, game.getBoard().toIndex(7, 4));
        game.addPiece(PieceType::Rook, PlayerColor::Red, game.getBoard().toIndex(7, 7));
        game.addPiece(PieceType::King, PlayerColor::Blue, game.getBoard().toIndex(0, 0));
        game.addPiece(PieceType::King, PlayerColor::Green, game.getBoard().toIndex(0, 11));
        game.setCurrentPlayer(PlayerColor::Red);

        game.printBoard(std::cout);

        const auto redMoves = game.getLegalMovesForPlayer(PlayerColor::Red);
        printMoveList("Coups legaux du joueur Red", redMoves);

        const int kingFrom = game.getBoard().toIndex(7, 4);
        const int kingTo = game.getBoard().toIndex(7, 6);
        const Move castlingMove = findMove(redMoves, kingFrom, kingTo, MoveType::Castling);

        std::cout << "Roque trouve ? " << ((castlingMove.getFrom() != nullptr) ? "oui" : "non") << "\n\n";

        if (castlingMove.getFrom() != nullptr)
        {
            game.playMove(castlingMove);
            std::cout << game.getLastEvent() << "\n\n";
            game.printBoard(std::cout);
        }
    }

    void scenarioEnPassant()
    {
        printHeader("SCENARIO 2 - Prise en passant avec fenetre de reponse sur 2 adversaires");

        Game game;
        game.addPiece(PieceType::King, PlayerColor::Red, game.getBoard().toIndex(7, 0));
        game.addPiece(PieceType::King, PlayerColor::Blue, game.getBoard().toIndex(0, 11));
        game.addPiece(PieceType::King, PlayerColor::Green, game.getBoard().toIndex(7, 11));

        game.addPiece(PieceType::Pawn, PlayerColor::Red, game.getBoard().toIndex(3, 5));
        game.addPiece(PieceType::Pawn, PlayerColor::Blue, game.getBoard().toIndex(1, 6));
        game.addPiece(PieceType::Knight, PlayerColor::Green, game.getBoard().toIndex(5, 10));

        game.setCurrentPlayer(PlayerColor::Blue);

        std::cout << "Etat initial :\n";
        game.printBoard(std::cout);

        const auto blueMoves = game.getLegalMovesForPlayer(PlayerColor::Blue);
        const Move blueDoubleStep = findMove(
            blueMoves,
            game.getBoard().toIndex(1, 6),
            game.getBoard().toIndex(3, 6),
            MoveType::Normal
        );

        if (blueDoubleStep.getFrom() != nullptr)
        {
            game.playMove(blueDoubleStep);
            std::cout << "\n" << game.getLastEvent() << "\n\n";
            game.printBoard(std::cout);
        }

        const auto greenMoves = game.getLegalMovesForPlayer(PlayerColor::Green);
        const Move greenReply = findMove(
            greenMoves,
            game.getBoard().toIndex(5, 10),
            game.getBoard().toIndex(3, 9),
            MoveType::Normal
        );

        if (greenReply.getFrom() != nullptr)
        {
            game.playMove(greenReply);
            std::cout << "\n" << game.getLastEvent() << "\n\n";
            game.printBoard(std::cout);
        }

        const auto redMoves = game.getLegalMovesForPlayer(PlayerColor::Red);
        printMoveList("Coups legaux du joueur Red apres la reponse de Green", redMoves);

        const Move enPassantMove = findMove(
            redMoves,
            game.getBoard().toIndex(3, 5),
            game.getBoard().toIndex(2, 6),
            MoveType::EnPassant
        );

        std::cout << "Prise en passant trouvee ? "
                  << ((enPassantMove.getFrom() != nullptr) ? "oui" : "non")
                  << "\n\n";

        if (enPassantMove.getFrom() != nullptr)
        {
            game.playMove(enPassantMove);
            std::cout << game.getLastEvent() << "\n\n";
            game.printBoard(std::cout);
        }
    }

    void scenarioPromotion()
    {
        printHeader("SCENARIO 3 - Promotion automatique parmi 4 choix proposes");

        Game game;
        game.addPiece(PieceType::King, PlayerColor::Red, game.getBoard().toIndex(7, 0));
        game.addPiece(PieceType::King, PlayerColor::Blue, game.getBoard().toIndex(0, 11));
        game.addPiece(PieceType::King, PlayerColor::Green, game.getBoard().toIndex(7, 11));
        game.addPiece(PieceType::Pawn, PlayerColor::Green, game.getBoard().toIndex(4, 1));

        game.setCurrentPlayer(PlayerColor::Green);

        game.printBoard(std::cout);
        const auto greenMoves = game.getLegalMovesForPlayer(PlayerColor::Green);
        printMoveList("Coups legaux du joueur Green", greenMoves);

        const Move promotionMove = findMove(
            greenMoves,
            game.getBoard().toIndex(4, 1),
            game.getBoard().toIndex(4, 0),
            MoveType::Promotion,
            PieceType::Queen
        );

        if (promotionMove.getFrom() != nullptr)
        {
            game.playMove(promotionMove);
            std::cout << game.getLastEvent() << "\n\n";
            game.printBoard(std::cout);
        }
    }

    void scenarioCheckmateAndElimination()
    {
        printHeader("SCENARIO 4 - Echec, mat et elimination d'un joueur");

        Game game;
        game.addPiece(PieceType::King, PlayerColor::Red, game.getBoard().toIndex(7, 0));
        game.addPiece(PieceType::King, PlayerColor::Blue, game.getBoard().toIndex(0, 11));
        game.addPiece(PieceType::King, PlayerColor::Green, game.getBoard().toIndex(0, 0));

        game.addPiece(PieceType::Rook, PlayerColor::Blue, game.getBoard().toIndex(7, 2));
        game.addPiece(PieceType::Bishop, PlayerColor::Blue, game.getBoard().toIndex(5, 2));
        game.addPiece(PieceType::Queen, PlayerColor::Blue, game.getBoard().toIndex(5, 0));

        game.setCurrentPlayer(PlayerColor::Blue);

        std::cout << "Avant le coup matant :\n";
        game.printBoard(std::cout);

        const auto blueMoves = game.getLegalMovesForPlayer(PlayerColor::Blue);
        const Move matingMove = findMove(
            blueMoves,
            game.getBoard().toIndex(5, 0),
            game.getBoard().toIndex(6, 1),
            MoveType::Normal
        );

        if (matingMove.getFrom() != nullptr)
        {
            game.playMove(matingMove);
            std::cout << "\n" << game.getLastEvent() << "\n";
            std::cout << "Etat du moteur : " << toString(game.getState()) << '\n';
            std::cout << "Joueur courant apres elimination : "
                      << (game.getCurrentPlayer() != nullptr ? game.getCurrentPlayer()->getName() : "aucun")
                      << "\n\n";
            game.printBoard(std::cout);
        }
    }
}

int main()
{
    std::cout << "projet-echec-yalta - S05 - Regles avancees\n";
    std::cout << "Moteur console de validation avant interface graphique SFML.\n";
    std::cout << "Les regles Yalta exactes du plateau restent a affiner plus tard ;\n";
    std::cout << "cette etape stabilise surtout le moteur, la legalite des coups et les cas speciaux.\n";

    scenarioCastling();
    scenarioEnPassant();
    scenarioPromotion();
    scenarioCheckmateAndElimination();

    return 0;
}