#include "mvc/SFMLView.hpp"

#include <cmath>

#include "core/Board.hpp"
#include "core/Game.hpp"
#include "core/Piece.hpp"
#include "core/Player.hpp"
#include "core/Square.hpp"

namespace
{
    sf::Color withAlpha(const sf::Color& color, std::uint8_t alpha)
    {
        return sf::Color(color.r, color.g, color.b, alpha);
    }
}

SFMLView::SFMLView()
    : m_window(sf::VideoMode({kWindowWidth, kWindowHeight}),
               "projet-echec-yalta - S08 - Interface graphique SFML")
{
}

sf::RenderWindow& SFMLView::getWindow()
{
    return m_window;
}

int SFMLView::squareIdFromPixel(sf::Vector2i pixel) const
{
    const float x = static_cast<float>(pixel.x);
    const float y = static_cast<float>(pixel.y);

    if (x < kBoardOffsetX || y < kBoardOffsetY)
    {
        return -1;
    }

    if (x >= kBoardOffsetX + kBoardWidth || y >= kBoardOffsetY + kBoardHeight)
    {
        return -1;
    }

    const int col = static_cast<int>((x - kBoardOffsetX) / kCellSize);
    const int row = static_cast<int>((y - kBoardOffsetY) / kCellSize);

    if (row < 0 || row >= 8 || col < 0 || col >= 12)
    {
        return -1;
    }

    return row * 12 + col;
}

void SFMLView::render(
    const Game& game,
    const Square* selectedSquare,
    const std::vector<Move>& highlightedMoves)
{
    m_window.clear(sf::Color(28, 30, 34));

    drawBoard(game, selectedSquare);
    drawHighlights(game, highlightedMoves);
    drawPieces(game);
    drawStatusPanel(game, selectedSquare);

    m_window.display();
}

void SFMLView::drawBoard(const Game& game, const Square* selectedSquare)
{
    const Board& board = game.getBoard();

    for (int row = 0; row < board.getRowCount(); ++row)
    {
        for (int col = 0; col < board.getColCount(); ++col)
        {
            sf::RectangleShape cell({kCellSize - 1.f, kCellSize - 1.f});
            cell.setPosition(cellTopLeft(row, col));
            cell.setFillColor(squareColor(row, col));

            const Square* square = board.getSquare(row, col);
            if (selectedSquare != nullptr
                && square != nullptr
                && selectedSquare->getId() == square->getId())
            {
                cell.setOutlineThickness(4.f);
                cell.setOutlineColor(sf::Color(255, 215, 0));
            }
            else
            {
                cell.setOutlineThickness(1.f);
                cell.setOutlineColor(sf::Color(45, 45, 45));
            }

            m_window.draw(cell);
        }
    }

    sf::RectangleShape border({kBoardWidth, kBoardHeight});
    border.setPosition({kBoardOffsetX, kBoardOffsetY});
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineThickness(3.f);
    border.setOutlineColor(sf::Color(200, 200, 200));
    m_window.draw(border);
}

void SFMLView::drawHighlights(const Game& game, const std::vector<Move>& highlightedMoves)
{
    const Board& board = game.getBoard();

    for (const Move& move : highlightedMoves)
    {
        if (move.getTo() == nullptr)
        {
            continue;
        }

        const int targetId = move.getTo()->getId();
        const int row = board.getRow(targetId);
        const int col = board.getCol(targetId);

        sf::RectangleShape overlay({kCellSize - 8.f, kCellSize - 8.f});
        overlay.setPosition(cellTopLeft(row, col) + sf::Vector2f(4.f, 4.f));

        if (move.getType() == MoveType::Capture || move.getType() == MoveType::EnPassant)
        {
            overlay.setFillColor(withAlpha(sf::Color::Red, 110));
        }
        else if (move.getType() == MoveType::Castling || move.getType() == MoveType::Promotion)
        {
            overlay.setFillColor(withAlpha(sf::Color(180, 90, 255), 110));
        }
        else
        {
            overlay.setFillColor(withAlpha(sf::Color::Green, 100));
        }

        overlay.setOutlineThickness(2.f);
        overlay.setOutlineColor(sf::Color::White);

        m_window.draw(overlay);
    }
}

void SFMLView::drawPieces(const Game& game)
{
    const Board& board = game.getBoard();

    for (const Square& square : board.getSquares())
    {
        if (!square.isOccupied())
        {
            continue;
        }

        Piece* piece = square.getPiece();
        if (piece == nullptr || piece->isCaptured())
        {
            continue;
        }

        const int row = board.getRow(square.getId());
        const int col = board.getCol(square.getId());

        drawPiece(*piece, cellCenter(row, col), 18.f);
    }
}

void SFMLView::drawPiece(const Piece& piece, sf::Vector2f center, float radius)
{
    const sf::Color baseColor = colorForPlayer(piece.getOwner()->getColor());
    const sf::Color markerColor = sf::Color(20, 20, 20);

    sf::CircleShape body(radius);
    body.setOrigin({radius, radius});
    body.setPosition(center);
    body.setFillColor(baseColor);
    body.setOutlineThickness(3.f);
    body.setOutlineColor(sf::Color(235, 235, 235));
    m_window.draw(body);

    switch (piece.getType())
    {
        case PieceType::Pawn:
        {
            sf::CircleShape marker(radius * 0.33f);
            marker.setOrigin({radius * 0.33f, radius * 0.33f});
            marker.setPosition(center);
            marker.setFillColor(markerColor);
            m_window.draw(marker);
            break;
        }

        case PieceType::Rook:
        {
            sf::RectangleShape top({radius * 1.2f, radius * 0.5f});
            top.setOrigin({radius * 0.6f, radius * 0.25f});
            top.setPosition(center + sf::Vector2f(0.f, -radius * 0.45f));
            top.setFillColor(markerColor);
            m_window.draw(top);

            sf::RectangleShape stem({radius * 0.8f, radius * 0.65f});
            stem.setOrigin({radius * 0.4f, radius * 0.325f});
            stem.setPosition(center + sf::Vector2f(0.f, radius * 0.1f));
            stem.setFillColor(markerColor);
            m_window.draw(stem);
            break;
        }

        case PieceType::Knight:
        {
            sf::CircleShape triangle(radius * 0.9f, 3);
            triangle.setOrigin({radius * 0.9f, radius * 0.9f});
            triangle.setPosition(center + sf::Vector2f(0.f, radius * 0.35f));
            triangle.setRotation(sf::degrees(-90.f));
            triangle.setFillColor(markerColor);
            m_window.draw(triangle);
            break;
        }

        case PieceType::Bishop:
        {
            drawLine(
                center + sf::Vector2f(-radius * 0.55f, radius * 0.55f),
                center + sf::Vector2f(radius * 0.55f, -radius * 0.55f),
                4.f,
                markerColor);

            sf::CircleShape notch(radius * 0.18f);
            notch.setOrigin({radius * 0.18f, radius * 0.18f});
            notch.setPosition(center + sf::Vector2f(0.f, -radius * 0.55f));
            notch.setFillColor(markerColor);
            m_window.draw(notch);
            break;
        }

        case PieceType::Queen:
        {
            for (int i = -1; i <= 1; ++i)
            {
                sf::CircleShape dot(radius * 0.18f);
                dot.setOrigin({radius * 0.18f, radius * 0.18f});
                dot.setPosition(center + sf::Vector2f(i * radius * 0.45f, -radius * 0.55f));
                dot.setFillColor(markerColor);
                m_window.draw(dot);
            }

            sf::RectangleShape base({radius * 1.2f, radius * 0.35f});
            base.setOrigin({radius * 0.6f, radius * 0.175f});
            base.setPosition(center + sf::Vector2f(0.f, radius * 0.45f));
            base.setFillColor(markerColor);
            m_window.draw(base);
            break;
        }

        case PieceType::King:
        {
            drawLine(
                center + sf::Vector2f(0.f, -radius * 0.8f),
                center + sf::Vector2f(0.f, radius * 0.35f),
                4.f,
                markerColor);

            drawLine(
                center + sf::Vector2f(-radius * 0.35f, -radius * 0.35f),
                center + sf::Vector2f(radius * 0.35f, -radius * 0.35f),
                4.f,
                markerColor);

            sf::RectangleShape base({radius * 1.0f, radius * 0.3f});
            base.setOrigin({radius * 0.5f, radius * 0.15f});
            base.setPosition(center + sf::Vector2f(0.f, radius * 0.5f));
            base.setFillColor(markerColor);
            m_window.draw(base);
            break;
        }

        default:
            break;
    }
}

void SFMLView::drawStatusPanel(const Game& game, const Square* selectedSquare)
{
    sf::RectangleShape panel({350.f, kBoardHeight});
    panel.setPosition({kBoardOffsetX + kBoardWidth + 40.f, kBoardOffsetY});
    panel.setFillColor(sf::Color(40, 44, 52));
    panel.setOutlineThickness(2.f);
    panel.setOutlineColor(sf::Color(180, 180, 180));
    m_window.draw(panel);

    const Player* currentPlayer = game.getCurrentPlayer();
    const sf::Color turnColor = (currentPlayer != nullptr)
        ? colorForPlayer(currentPlayer->getColor())
        : sf::Color(120, 120, 120);

    sf::RectangleShape turnBanner({300.f, 40.f});
    turnBanner.setPosition({kBoardOffsetX + kBoardWidth + 65.f, kBoardOffsetY + 25.f});
    turnBanner.setFillColor(turnColor);
    m_window.draw(turnBanner);

    const PlayerColor colors[3] = {
        PlayerColor::Red,
        PlayerColor::Blue,
        PlayerColor::Green
    };

    for (int i = 0; i < 3; ++i)
    {
        sf::CircleShape indicator(24.f);
        indicator.setOrigin({24.f, 24.f});
        indicator.setPosition({
            kBoardOffsetX + kBoardWidth + 90.f + (i * 90.f),
            kBoardOffsetY + 110.f
        });
        indicator.setFillColor(colorForPlayer(colors[i]));
        indicator.setOutlineThickness(3.f);
        indicator.setOutlineColor(
            (currentPlayer != nullptr && currentPlayer->getColor() == colors[i])
                ? sf::Color::White
                : sf::Color(70, 70, 70));
        m_window.draw(indicator);
    }

    const Board& board = game.getBoard();
    int selectedId = -1;
    if (selectedSquare != nullptr)
    {
        selectedId = selectedSquare->getId();
    }

    for (int i = 0; i < 4; ++i)
    {
        sf::RectangleShape infoBox({300.f, 70.f});
        infoBox.setPosition({
            kBoardOffsetX + kBoardWidth + 65.f,
            kBoardOffsetY + 180.f + (i * 90.f)
        });
        infoBox.setFillColor((i % 2 == 0) ? sf::Color(55, 59, 67) : sf::Color(48, 52, 60));
        infoBox.setOutlineThickness(1.f);
        infoBox.setOutlineColor(sf::Color(100, 100, 100));
        m_window.draw(infoBox);
    }

    if (selectedId >= 0)
    {
        const int row = board.getRow(selectedId);
        const int col = board.getCol(selectedId);

        sf::RectangleShape marker({80.f, 80.f});
        marker.setPosition({kBoardOffsetX + kBoardWidth + 175.f, kBoardOffsetY + 535.f});
        marker.setFillColor(squareColor(row, col));
        marker.setOutlineThickness(4.f);
        marker.setOutlineColor(sf::Color(255, 215, 0));
        m_window.draw(marker);

        if (selectedSquare->isOccupied() && selectedSquare->getPiece() != nullptr)
        {
            drawPiece(*selectedSquare->getPiece(),
                      {kBoardOffsetX + kBoardWidth + 215.f, kBoardOffsetY + 575.f},
                      22.f);
        }
    }
}

sf::Color SFMLView::colorForPlayer(PlayerColor color) const
{
    switch (color)
    {
        case PlayerColor::Red:
            return sf::Color(210, 70, 70);
        case PlayerColor::Blue:
            return sf::Color(70, 120, 220);
        case PlayerColor::Green:
            return sf::Color(70, 180, 110);
        default:
            return sf::Color(150, 150, 150);
    }
}

sf::Color SFMLView::squareColor(int row, int col) const
{
    const bool dark = ((row + col) % 2 == 0);
    return dark ? sf::Color(118, 150, 86) : sf::Color(238, 238, 210);
}

sf::Vector2f SFMLView::cellTopLeft(int row, int col) const
{
    return {
        kBoardOffsetX + (static_cast<float>(col) * kCellSize),
        kBoardOffsetY + (static_cast<float>(row) * kCellSize)
    };
}

sf::Vector2f SFMLView::cellCenter(int row, int col) const
{
    return cellTopLeft(row, col) + sf::Vector2f(kCellSize * 0.5f, kCellSize * 0.5f);
}

void SFMLView::drawLine(
    sf::Vector2f start,
    sf::Vector2f end,
    float thickness,
    sf::Color color)
{
    const sf::Vector2f delta = end - start;
    const float length = std::sqrt((delta.x * delta.x) + (delta.y * delta.y));
    const float angleDegrees = std::atan2(delta.y, delta.x) * 180.f / 3.1415926535f;

    sf::RectangleShape line({length, thickness});
    line.setOrigin({0.f, thickness * 0.5f});
    line.setPosition(start);
    line.setRotation(sf::degrees(angleDegrees));
    line.setFillColor(color);

    m_window.draw(line);
}