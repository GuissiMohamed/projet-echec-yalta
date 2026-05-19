#include <cstdlib>
#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "core/Game.hpp"
#include "core/Piece.hpp"
#include "core/Square.hpp"

namespace
{
    int g_failures = 0;
    int g_checks = 0;

    void check(bool condition, const std::string& message)
    {
        ++g_checks;

        if (!condition)
        {
            ++g_failures;
            std::cerr << "[FAIL] " << message << '\n';
        }
    }

    void checkEqual(int actual, int expected, const std::string& message)
    {
        std::ostringstream stream;
        stream << message << " (expected=" << expected << ", actual=" << actual << ")";
        check(actual == expected, stream.str());
    }

    void checkEqual(bool actual, bool expected, const std::string& message)
    {
        std::ostringstream stream;
        stream << message << " (expected=" << (expected ? "true" : "false")
               << ", actual=" << (actual ? "true" : "false") << ")";
        check(actual == expected, stream.str());
    }

    void checkEqual(PlayerColor actual, PlayerColor expected, const std::string& message)
    {
        std::ostringstream stream;
        stream << message << " (expected=" << toString(expected)
               << ", actual=" << toString(actual) << ")";
        check(actual == expected, stream.str());
    }

    Piece* requirePiece(Game& game, PieceType type, PlayerColor color, int squareId, const std::string& context)
    {
        Piece* piece = game.addPiece(type, color, squareId);
        check(piece != nullptr, context + " : addPiece a retourne nullptr");
        return piece;
    }

    void addReferenceKings(Game& game)
    {
        requirePiece(game, PieceType::King, PlayerColor::Red, game.getBoard().toIndex(7, 0), "Roi rouge");
        requirePiece(game, PieceType::King, PlayerColor::Blue, game.getBoard().toIndex(0, 0), "Roi bleu");
        requirePiece(game, PieceType::King, PlayerColor::Green, game.getBoard().toIndex(4, 11), "Roi vert");
    }

    bool containsMoveTo(const std::vector<Move>& moves, int toId, MoveType type = MoveType::Normal)
    {
        for (const Move& move : moves)
        {
            if (move.getToId() == toId && move.getType() == type)
            {
                return true;
            }
        }

        return false;
    }

    int countPromotionMovesTo(const std::vector<Move>& moves, int toId)
    {
        int count = 0;

        for (const Move& move : moves)
        {
            if (move.getToId() == toId && move.getType() == MoveType::Promotion)
            {
                ++count;
            }
        }

        return count;
    }

    void testBoardHas96Squares()
    {
        Game game;
        checkEqual(game.getBoard().getSquareCount(), 96, "Le plateau doit contenir 96 cases");
        checkEqual(game.getBoard().getRowCount(), 8, "Le plateau doit exposer 8 lignes logiques");
        checkEqual(game.getBoard().getColCount(), 12, "Le plateau doit exposer 12 colonnes logiques");
    }

    void testAddPieceOnFreeSquare()
    {
        Game game;
        Piece* piece = game.addPiece(PieceType::King, PlayerColor::Red, 10);

        check(piece != nullptr, "Ajout d'une piece sur une case libre");
        check(game.getBoard().getSquare(10) != nullptr, "La case 10 doit exister");
        check(game.getBoard().getSquare(10)->isOccupied(), "La case 10 doit etre occupee apres ajout");
        check(game.getBoard().getSquare(10)->getPiece() == piece, "La case 10 doit contenir la piece ajoutee");
    }

    void testAddPieceOnOccupiedSquareFails()
    {
        Game game;
        Piece* first = game.addPiece(PieceType::King, PlayerColor::Red, 12);
        Piece* second = game.addPiece(PieceType::Queen, PlayerColor::Blue, 12);

        check(first != nullptr, "Premiere piece ajoutee sur case 12");
        check(second == nullptr, "Ajout refuse sur case deja occupee");
    }

    void testGameCopyPreservesBoardState()
    {
        Game game;
        requirePiece(game, PieceType::King, PlayerColor::Red, 88, "Copie");
        requirePiece(game, PieceType::Queen, PlayerColor::Blue, 4, "Copie");
        requirePiece(game, PieceType::Pawn, PlayerColor::Green, 47, "Copie");

        Game copy(game);

        check(copy.getBoard().getSquare(88) != nullptr, "La copie doit contenir la case 88");
        check(copy.getBoard().getSquare(88)->isOccupied(), "Le roi rouge doit exister dans la copie");
        check(copy.getBoard().getSquare(4)->isOccupied(), "La reine bleue doit exister dans la copie");
        check(copy.getBoard().getSquare(47)->isOccupied(), "Le pion vert doit exister dans la copie");

        Piece* redKingCopy = copy.getBoard().getSquare(88)->getPiece();
        check(redKingCopy != nullptr, "La piece copiee doit etre accessible");
        checkEqual(redKingCopy->getOwner()->getColor(), PlayerColor::Red, "Le roi copie doit appartenir a Red");
    }

    void testRedPawnInitialMoves()
    {
        Game game;
        addReferenceKings(game);

        Piece* pawn = requirePiece(game, PieceType::Pawn, PlayerColor::Red, game.getBoard().toIndex(6, 4), "Pion rouge");
        const std::vector<Move> moves = game.getLegalMovesForPiece(*pawn);

        checkEqual(static_cast<int>(moves.size()), 2, "Un pion rouge initial doit avoir 2 coups");
        check(containsMoveTo(moves, game.getBoard().toIndex(5, 4)), "Le pion rouge doit pouvoir avancer d'une case");
        check(containsMoveTo(moves, game.getBoard().toIndex(4, 4)), "Le pion rouge doit pouvoir avancer de deux cases");
    }

    void testBluePawnInitialMoves()
    {
        Game game;
        addReferenceKings(game);

        Piece* pawn = requirePiece(game, PieceType::Pawn, PlayerColor::Blue, game.getBoard().toIndex(1, 4), "Pion bleu");
        const std::vector<Move> moves = game.getLegalMovesForPiece(*pawn);

        checkEqual(static_cast<int>(moves.size()), 2, "Un pion bleu initial doit avoir 2 coups");
        check(containsMoveTo(moves, game.getBoard().toIndex(2, 4)), "Le pion bleu doit avancer vers le bas");
        check(containsMoveTo(moves, game.getBoard().toIndex(3, 4)), "Le pion bleu doit pouvoir avancer de deux cases");
    }

    void testGreenPawnInitialMoves()
    {
        Game game;
        addReferenceKings(game);

        Piece* pawn = requirePiece(game, PieceType::Pawn, PlayerColor::Green, game.getBoard().toIndex(4, 10), "Pion vert");
        const std::vector<Move> moves = game.getLegalMovesForPiece(*pawn);

        checkEqual(static_cast<int>(moves.size()), 2, "Un pion vert initial doit avoir 2 coups");
        check(containsMoveTo(moves, game.getBoard().toIndex(4, 9)), "Le pion vert doit avancer vers la gauche");
        check(containsMoveTo(moves, game.getBoard().toIndex(4, 8)), "Le pion vert doit pouvoir avancer de deux cases");
    }

    void testPromotionGeneration()
    {
        Game game;
        addReferenceKings(game);

        Piece* pawn = requirePiece(game, PieceType::Pawn, PlayerColor::Red, game.getBoard().toIndex(1, 4), "Promotion");
        const std::vector<Move> moves = game.getLegalMovesForPiece(*pawn);

        const int promotionTarget = game.getBoard().toIndex(0, 4);
        checkEqual(countPromotionMovesTo(moves, promotionTarget), 4, "La promotion doit proposer 4 pieces");
    }

    void testCheckDetectionWithRook()
    {
        Game game;
        requirePiece(game, PieceType::King, PlayerColor::Red, game.getBoard().toIndex(7, 4), "Echec");
        requirePiece(game, PieceType::King, PlayerColor::Blue, game.getBoard().toIndex(0, 0), "Echec");
        requirePiece(game, PieceType::King, PlayerColor::Green, game.getBoard().toIndex(4, 11), "Echec");
        requirePiece(game, PieceType::Rook, PlayerColor::Blue, game.getBoard().toIndex(0, 4), "Echec");

        checkEqual(game.isPlayerInCheck(PlayerColor::Red), true, "Le roi rouge doit etre en echec par la tour bleue");
        checkEqual(game.isPlayerInCheck(PlayerColor::Blue), false, "Le joueur bleu ne doit pas etre en echec");
    }

    void testPlayMoveAdvancesTurn()
    {
        Game game;
        Piece* pawn = requirePiece(game, PieceType::Pawn, PlayerColor::Red, game.getBoard().toIndex(6, 4), "Tour");
        requirePiece(game, PieceType::King, PlayerColor::Red, game.getBoard().toIndex(7, 0), "Tour");
        requirePiece(game, PieceType::King, PlayerColor::Blue, game.getBoard().toIndex(0, 0), "Tour");
        requirePiece(game, PieceType::King, PlayerColor::Green, game.getBoard().toIndex(4, 11), "Tour");

        game.setCurrentPlayer(PlayerColor::Red);

        const std::vector<Move> moves = game.getLegalMovesForPiece(*pawn);
        check(!moves.empty(), "Le pion rouge doit avoir au moins un coup");

        bool played = false;
        for (const Move& move : moves)
        {
            if (move.getToId() == game.getBoard().toIndex(5, 4))
            {
                played = game.playMove(move);
                break;
            }
        }

        checkEqual(played, true, "Le coup rouge doit etre accepte");
        check(game.getBoard().getSquare(game.getBoard().toIndex(5, 4))->isOccupied(),
              "Le pion rouge doit se retrouver sur sa nouvelle case");
        checkEqual(game.getCurrentPlayer()->getColor(), PlayerColor::Blue,
                   "Apres le coup rouge, le tour doit passer a Blue");
    }
}

int main()
{
    try
    {
        testBoardHas96Squares();
        testAddPieceOnFreeSquare();
        testAddPieceOnOccupiedSquareFails();
        testGameCopyPreservesBoardState();
        testRedPawnInitialMoves();
        testBluePawnInitialMoves();
        testGreenPawnInitialMoves();
        testPromotionGeneration();
        testCheckDetectionWithRook();
        testPlayMoveAdvancesTurn();
    }
    catch (const std::exception& exception)
    {
        std::cerr << "[EXCEPTION] " << exception.what() << '\n';
        return EXIT_FAILURE;
    }
    catch (...)
    {
        std::cerr << "[EXCEPTION] Unknown exception\n";
        return EXIT_FAILURE;
    }

    if (g_failures > 0)
    {
        std::cerr << "\n" << g_failures << " test(s) en echec sur " << g_checks << " verification(s).\n";
        return EXIT_FAILURE;
    }

    std::cout << "Tous les tests ont reussi (" << g_checks << " verifications).\n";
    return EXIT_SUCCESS;
}