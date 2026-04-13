#include "mvc/GameController.hpp"

#include <string>

#include "core/Piece.hpp"
#include "core/Square.hpp"
#include "mvc/ConsoleView.hpp"

GameController::GameController(Game& game, ConsoleView& view)
    : m_game(game), m_view(view), m_userRequestedQuit(false)
{
}

void GameController::placeHorizontalArmy(PlayerColor color, int backRow, int pawnRow)
{
    const PieceType backRank[8] = {
        PieceType::Rook,
        PieceType::Knight,
        PieceType::Bishop,
        PieceType::Queen,
        PieceType::King,
        PieceType::Bishop,
        PieceType::Knight,
        PieceType::Rook
    };

    for (int col = 0; col < 8; ++col)
    {
        m_game.addPiece(backRank[col], color, m_game.getBoard().toIndex(backRow, col));
        m_game.addPiece(PieceType::Pawn, color, m_game.getBoard().toIndex(pawnRow, col));
    }
}

void GameController::placeVerticalArmy(PlayerColor color, int backCol, int pawnCol)
{
    const PieceType backRank[8] = {
        PieceType::Rook,
        PieceType::Knight,
        PieceType::Bishop,
        PieceType::Queen,
        PieceType::King,
        PieceType::Bishop,
        PieceType::Knight,
        PieceType::Rook
    };

    for (int row = 0; row < 8; ++row)
    {
        m_game.addPiece(backRank[row], color, m_game.getBoard().toIndex(row, backCol));
        m_game.addPiece(PieceType::Pawn, color, m_game.getBoard().toIndex(row, pawnCol));
    }
}

void GameController::setupDefaultPosition()
{
    placeHorizontalArmy(PlayerColor::Blue, 0, 1);
    placeHorizontalArmy(PlayerColor::Red, 7, 6);
    placeVerticalArmy(PlayerColor::Green, 11, 10);

    m_game.setCurrentPlayer(PlayerColor::Red);

    m_view.showMessage("Position initiale simplifiee chargee.");
    m_view.showMessage("Attention : ce placement reste temporaire par rapport a la vraie geometrie Yalta.");
    m_view.showMessage("");
}

bool GameController::isSessionFinished() const
{
    if (m_userRequestedQuit)
    {
        return true;
    }

    const GameState state = m_game.getState();

    return state == GameState::Draw
        || state == GameState::Stalemate
        || state == GameState::WinByElimination;
}

std::vector<Move> GameController::filterMovesByFrom(const std::vector<Move>& moves, int fromId) const
{
    std::vector<Move> result;

    for (const Move& move : moves)
    {
        if (move.getFrom() != nullptr && move.getFrom()->getId() == fromId)
        {
            result.push_back(move);
        }
    }

    return result;
}

std::vector<Move> GameController::filterMovesByTo(const std::vector<Move>& moves, int toId) const
{
    std::vector<Move> result;

    for (const Move& move : moves)
    {
        if (move.getTo() != nullptr && move.getTo()->getId() == toId)
        {
            result.push_back(move);
        }
    }

    return result;
}

Move GameController::selectPromotionMove(const std::vector<Move>& promotionMoves) const
{
    const int choice = m_view.askPromotionChoice();

    PieceType selectedType = PieceType::Queen;

    switch (choice)
    {
        case 1: selectedType = PieceType::Queen; break;
        case 2: selectedType = PieceType::Rook; break;
        case 3: selectedType = PieceType::Bishop; break;
        case 4: selectedType = PieceType::Knight; break;
        default: selectedType = PieceType::Queen; break;
    }

    for (const Move& move : promotionMoves)
    {
        if (move.getPromotionType() == selectedType)
        {
            return move;
        }
    }

    return {};
}

bool GameController::handleTurn()
{
    Player* currentPlayer = m_game.getCurrentPlayer();

    if (currentPlayer == nullptr)
    {
        m_view.showMessage("Aucun joueur courant disponible.");
        return false;
    }

    const std::vector<Move> playerMoves = m_game.getLegalMovesForPlayer(currentPlayer->getColor());

    if (playerMoves.empty())
    {
        m_view.showMessage("Le joueur courant n'a aucun coup legal.");
        return false;
    }

    while (true)
    {
        const std::string command = m_view.askCommand();

        if (command == "quit")
        {
            m_userRequestedQuit = true;
            m_view.showMessage("Session interrompue par l'utilisateur.");
            return false;
        }

        if (command != "play")
        {
            m_view.showMessage("Commande inconnue.");
            continue;
        }

        const int fromId = m_view.askSquareId("Case source : ");
        const std::vector<Move> pieceMoves = filterMovesByFrom(playerMoves, fromId);

        if (pieceMoves.empty())
        {
            m_view.showMessage("Aucun coup legal depuis cette case.");
            continue;
        }

        m_view.showMoves(pieceMoves);

        const int toId = m_view.askSquareId("Case destination : ");
        const std::vector<Move> destinationMoves = filterMovesByTo(pieceMoves, toId);

        if (destinationMoves.empty())
        {
            m_view.showMessage("Destination invalide pour cette piece.");
            continue;
        }

        Move selectedMove = destinationMoves.front();

        if (destinationMoves.size() > 1 && destinationMoves.front().getType() == MoveType::Promotion)
        {
            selectedMove = selectPromotionMove(destinationMoves);
            if (selectedMove.getFrom() == nullptr)
            {
                m_view.showMessage("Promotion invalide.");
                continue;
            }
        }

        if (!m_game.playMove(selectedMove))
        {
            m_view.showMessage("Le coup a ete refuse par le moteur.");
            continue;
        }

        m_view.showMessage("");
        m_view.showMessage("Coup joue avec succes.");
        m_view.showMessage(m_game.getLastEvent());
        m_view.showMessage("");
        return true;
    }
}

void GameController::run()
{
    m_view.showWelcome();
    setupDefaultPosition();

    while (!isSessionFinished())
    {
        m_view.showBoard(m_game);
        m_view.showStatus(m_game);

        if (!handleTurn())
        {
            break;
        }
    }

    m_view.showBoard(m_game);
    m_view.showEnd(m_game);
}