#include "mvc/SFMLGameController.hpp"

#include <optional>

#include <SFML/Graphics.hpp>

#include "core/Piece.hpp"
#include "core/Square.hpp"
#include "mvc/SFMLView.hpp"
#include "patterns/MoveCommand.hpp"

SFMLGameController::SFMLGameController(Game& game, SFMLView& view)
    : m_game(game),
      m_view(view),
      m_selectedSquare(nullptr)
{
}

void SFMLGameController::placeHorizontalArmy(PlayerColor color, int backRow, int pawnRow)
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

void SFMLGameController::placeVerticalArmy(PlayerColor color, int backCol, int pawnCol)
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

void SFMLGameController::setupDefaultPosition()
{
    placeHorizontalArmy(PlayerColor::Blue, 0, 1);
    placeHorizontalArmy(PlayerColor::Red, 7, 6);
    placeVerticalArmy(PlayerColor::Green, 11, 10);

    m_game.setCurrentPlayer(PlayerColor::Red);
}

void SFMLGameController::resetSelection()
{
    m_selectedSquare = nullptr;
    m_selectedMoves.clear();
}

void SFMLGameController::selectSquare(Square* square)
{
    resetSelection();

    if (square == nullptr || !square->isOccupied())
    {
        return;
    }

    Piece* piece = square->getPiece();
    if (piece == nullptr || piece->isCaptured())
    {
        return;
    }

    Player* currentPlayer = m_game.getCurrentPlayer();
    if (currentPlayer == nullptr)
    {
        return;
    }

    if (piece->getOwner() != currentPlayer)
    {
        return;
    }

    m_selectedSquare = square;
    m_selectedMoves = m_game.getLegalMovesForPiece(*piece);

    if (m_selectedMoves.empty())
    {
        resetSelection();
    }
}

bool SFMLGameController::tryPlayMoveTo(int targetSquareId)
{
    std::vector<Move> destinationMatches;

    for (const Move& move : m_selectedMoves)
    {
        if (move.getTo() != nullptr && move.getTo()->getId() == targetSquareId)
        {
            destinationMatches.push_back(move);
        }
    }

    if (destinationMatches.empty())
    {
        return false;
    }

    Move selectedMove = destinationMatches.front();

    // Simplification temporaire S08 :
    // si plusieurs promotions existent sur la meme case,
    // on choisit automatiquement la reine.
    if (destinationMatches.size() > 1)
    {
        for (const Move& move : destinationMatches)
        {
            if (move.getType() == MoveType::Promotion
                && move.getPromotionType() == PieceType::Queen)
            {
                selectedMove = move;
                break;
            }
        }
    }

    MoveCommand command(m_game, selectedMove);
    const bool success = command.execute();

    resetSelection();
    return success;
}

void SFMLGameController::handleBoardClick(int squareId)
{
    if (squareId < 0)
    {
        resetSelection();
        return;
    }

    Square* clickedSquare = m_game.getBoard().getSquare(squareId);
    if (clickedSquare == nullptr)
    {
        resetSelection();
        return;
    }

    Player* currentPlayer = m_game.getCurrentPlayer();
    if (currentPlayer == nullptr)
    {
        resetSelection();
        return;
    }

    if (m_selectedSquare == nullptr)
    {
        selectSquare(clickedSquare);
        return;
    }

    if (clickedSquare == m_selectedSquare)
    {
        resetSelection();
        return;
    }

    if (clickedSquare->isOccupied()
        && clickedSquare->getPiece() != nullptr
        && clickedSquare->getPiece()->getOwner() == currentPlayer)
    {
        selectSquare(clickedSquare);
        return;
    }

    const bool played = tryPlayMoveTo(squareId);
    if (!played)
    {
        resetSelection();
    }
}

void SFMLGameController::run()
{
    setupDefaultPosition();

    sf::RenderWindow& window = m_view.getWindow();

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }

            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                if (keyPressed->code == sf::Keyboard::Key::Escape)
                {
                    window.close();
                }
            }

            if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mousePressed->button == sf::Mouse::Button::Left)
                {
                    const sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
                    const int squareId = m_view.squareIdFromPixel(mousePosition);
                    handleBoardClick(squareId);
                }

                if (mousePressed->button == sf::Mouse::Button::Right)
                {
                    resetSelection();
                }
            }
        }

        m_view.render(m_game, m_selectedSquare, m_selectedMoves);
    }
}