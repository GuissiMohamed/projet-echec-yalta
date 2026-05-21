/**
 * @class Game
 * @brief Classe principale du moteur du jeu Yalta
 *
 * Cette classe gère :
 * - le plateau
 * - les joueurs
 * - les pièces
 * - les règles du jeu
 * - les coups
 * - les états de la partie (échec, victoire, nulle)
 */

#pragma once

#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

#include "core/Board.hpp"
#include "core/Move.hpp"
#include "core/Player.hpp"
#include "core/Types.hpp"

class Piece;
class Square;

class Game
{
public:
    /**
     * @brief Constructeur du jeu
     */
    Game();

    /**
     * @brief Joue un coup s'il est valide
     * @param move coup à exécuter
     * @return true si le coup a été joué
     */
    Game(const Game& other);
    ~Game();

    Game& operator=(const Game&) = delete;

    Board& getBoard();
    const Board& getBoard() const;

    Player* getPlayer(PlayerColor color);
    const Player* getPlayer(PlayerColor color) const;

    Player* getCurrentPlayer();
    const Player* getCurrentPlayer() const;
    void setCurrentPlayer(PlayerColor color);

    Piece* addPiece(PieceType type, PlayerColor color, int squareId);
    Piece* getKing(PlayerColor color) const;

    std::vector<Move> getLegalMovesForPiece(const Piece& piece);
    std::vector<Move> getLegalMovesForPlayer(PlayerColor color);

    bool isPlayerInCheck(PlayerColor color) const;
    bool hasAnyLegalMove(PlayerColor color);
    bool isPlayerCheckmated(PlayerColor color);
    bool isPlayerStalemated(PlayerColor color);

    bool playMove(const Move& move);

    /**
     * @brief Retourne tous les coups légaux d’un joueur
     */

    int getActivePlayerCount() const;
    GameState getState() const;
    PlayerColor getWinner() const;
    const std::string& getLastEvent() const;

    void printBoard(std::ostream& output) const;

private:
    struct MoveStateSnapshot
    {
        Piece* movedPiece = nullptr;
        PieceType movedPieceOriginalType = PieceType::None;
        bool movedPieceOriginalHasMoved = false;
        Square* fromSquare = nullptr;
        Square* toSquare = nullptr;

        Piece* capturedPiece = nullptr;
        Square* capturedPieceOriginalSquare = nullptr;
        bool capturedPieceOriginalCapturedState = false;

        Piece* rookPiece = nullptr;
        Square* rookFromSquare = nullptr;
        Square* rookToSquare = nullptr;
        bool rookOriginalHasMoved = false;

        int enPassantTargetBefore = -1;
        int enPassantVictimBefore = -1;
        int enPassantRemainingBefore = 0;
    };

    std::vector<Move> getCandidateMovesForPiece(const Piece& piece);
    std::vector<Move> getSpecialMovesForPiece(const Piece& piece);

    bool doesPieceAttackSquare(const Piece& piece, const Square& target) const;
    bool isSquareAttackedByOpponents(const Square& square, PlayerColor defenderColor) const;
    bool isPromotionSquare(PlayerColor color, const Square& square) const;
    bool isInsufficientMaterialDraw() const;
    bool wouldCaptureEnemyKing(const Move& move, PlayerColor movingColor) const;

    bool applyMoveInternal(const Move& move, MoveStateSnapshot& snapshot);
    void undoMoveInternal(const MoveStateSnapshot& snapshot);

    void updateEnPassantStateAfterMove(const MoveStateSnapshot& snapshot, const Move& move);
    void clearEnPassantState();
    void advanceToNextActivePlayer();
    void eliminatePlayer(Player& player);
    void eliminateCheckmatedPlayers();
    void refreshGameStateAfterMove();

    Player* findRemainingActivePlayer() const;

    Board m_board;
    std::vector<std::unique_ptr<Player>> m_players;
    std::vector<std::unique_ptr<Piece>> m_pieces;

    std::size_t m_currentPlayerIndex;
    GameState m_state;
    PlayerColor m_winner;
    std::string m_lastEvent;

    int m_enPassantTargetSquareId;
    int m_enPassantVictimSquareId;
    int m_enPassantRemainingResponses;
};