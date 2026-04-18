#include "mvc/GameController.hpp"

#include <string>

#include "mvc/ConsoleView.hpp"
#include "patterns/MoveCommand.hpp"
#include "strategy/IMoveStrategy.hpp"

GameController::GameController(
    Game& game,
    ConsoleView& view,
    std::unique_ptr<IMoveStrategy> redStrategy,
    std::unique_ptr<IMoveStrategy> blueStrategy,
    std::unique_ptr<IMoveStrategy> greenStrategy)
    : m_game(game),
      m_view(view),
      m_redStrategy(std::move(redStrategy)),
      m_blueStrategy(std::move(blueStrategy)),
      m_greenStrategy(std::move(greenStrategy)),
      m_userRequestedQuit(false)
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
    m_view.showMessage("S07 : MVC consolide + Command + Strategy.");
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

IMoveStrategy* GameController::getStrategy(PlayerColor color) const
{
    switch (color)
    {
        case PlayerColor::Red:
            return m_redStrategy.get();
        case PlayerColor::Blue:
            return m_blueStrategy.get();
        case PlayerColor::Green:
            return m_greenStrategy.get();
        default:
            return nullptr;
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

        Player* currentPlayer = m_game.getCurrentPlayer();
        if (currentPlayer == nullptr)
        {
            m_view.showMessage("Aucun joueur courant disponible.");
            break;
        }

        IMoveStrategy* strategy = getStrategy(currentPlayer->getColor());
        if (strategy == nullptr)
        {
            m_view.showMessage("Aucune strategie configuree pour ce joueur.");
            break;
        }

        m_view.showMessage("Strategie active : " + strategy->getName());

        StrategyDecision decision = strategy->chooseMove(
            m_game,
            m_view,
            currentPlayer->getColor());

        if (decision.quitRequested)
        {
            m_userRequestedQuit = true;
            m_view.showMessage("Session interrompue par l'utilisateur.");
            break;
        }

        if (!decision.selectedMove.has_value())
        {
            m_view.showMessage("La strategie n'a fourni aucun coup.");
            break;
        }

        MoveCommand command(m_game, *decision.selectedMove);
        m_view.showMessage(command.getLabel());

        if (!command.execute())
        {
            m_view.showMessage("Le coup a ete refuse par le moteur.");
            break;
        }

        m_view.showMessage("Coup joue avec succes.");
        m_view.showMessage(m_game.getLastEvent());
        m_view.showMessage("");
    }

    m_view.showBoard(m_game);
    m_view.showEnd(m_game);
}