#include "mvc/SFMLView.hpp"

#include <cmath>
#include <vector>

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

    float cross(sf::Vector2f a, sf::Vector2f b, sf::Vector2f c)
    {
        const sf::Vector2f ab = b - a;
        const sf::Vector2f ac = c - a;
        return (ab.x * ac.y) - (ab.y * ac.x);
    }

    // Maillage visuel explicite 9 x 13.
    // Chaque ligne correspond à une frontière horizontale du plateau,
    // chaque colonne à une frontière verticale.
    // Le but est de se rapprocher visuellement de la photo réelle,
    // sans prétendre encore modéliser la topologie logique exacte.
    constexpr std::array<float, 9> kGridY = {
        36.f, 108.f, 184.f, 270.f, 366.f, 468.f, 566.f, 652.f, 744.f
    };

    constexpr std::array<std::array<float, 13>, 9> kGridX = {{
        {{184.f, 241.f, 299.f, 356.f, 414.f, 471.f, 528.f, 585.f, 643.f, 700.f, 757.f, 815.f, 872.f}},
        {{147.f, 209.f, 272.f, 334.f, 396.f, 461.f, 528.f, 595.f, 660.f, 722.f, 784.f, 847.f, 909.f}},
        {{113.f, 181.f, 248.f, 315.f, 382.f, 455.f, 528.f, 601.f, 674.f, 741.f, 808.f, 875.f, 943.f}},
        {{84.f, 156.f, 228.f, 300.f, 372.f, 450.f, 528.f, 606.f, 684.f, 756.f, 828.f, 900.f, 972.f}},
        {{60.f, 136.f, 212.f, 288.f, 364.f, 446.f, 528.f, 610.f, 692.f, 768.f, 844.f, 920.f, 996.f}},
        {{86.f, 160.f, 234.f, 308.f, 382.f, 455.f, 528.f, 601.f, 674.f, 748.f, 822.f, 896.f, 970.f}},
        {{118.f, 186.f, 254.f, 322.f, 390.f, 459.f, 528.f, 597.f, 666.f, 734.f, 802.f, 870.f, 938.f}},
        {{151.f, 213.f, 275.f, 337.f, 399.f, 463.f, 528.f, 593.f, 657.f, 719.f, 781.f, 843.f, 905.f}},
        {{184.f, 241.f, 299.f, 356.f, 414.f, 471.f, 528.f, 585.f, 643.f, 700.f, 757.f, 815.f, 872.f}}
    }};
}

SFMLView::SFMLView()
    : m_window(sf::VideoMode({kWindowWidth, kWindowHeight}),
               "projet-echec-yalta - SFML")
{
    tryLoadFont();
}

sf::RenderWindow& SFMLView::getWindow()
{
    return m_window;
}

int SFMLView::squareIdFromPixel(const Game& game, sf::Vector2i pixel) const
{
    const Board& board = game.getBoard();
    const sf::Vector2f point(static_cast<float>(pixel.x), static_cast<float>(pixel.y));

    for (const Square& square : board.getSquares())
    {
        if (!board.isActiveSquare(square.getId()))
        {
            continue;
        }

        const Quad quad = cellQuad(square.getId());
        if (pointInQuad(point, quad))
        {
            return square.getId();
        }
    }

    return -1;
}

void SFMLView::render(
    const Game& game,
    const Square* selectedSquare,
    const std::vector<Move>& highlightedMoves)
{
    m_window.clear(sf::Color(25, 29, 38));

    drawBoard(game, selectedSquare);
    drawHighlights(game, highlightedMoves);
    drawPieces(game);
    drawStatusPanel(game, selectedSquare);

    if (game.getState() == GameState::Draw
        || game.getState() == GameState::Stalemate
        || game.getState() == GameState::WinByElimination)
    {
        drawEndgameOverlay(game);
    }

    m_window.display();
}

void SFMLView::drawBoard(const Game& game, const Square* selectedSquare)
{
    const Board& board = game.getBoard();

    for (const Square& square : board.getSquares())
    {
        if (!board.isActiveSquare(square.getId()))
        {
            continue;
        }

        const int row = board.getRow(square.getId());
        const int col = board.getCol(square.getId());

        sf::Color fill = squareColor(row, col);
        float outlineThickness = 1.0f;
        sf::Color outline = sf::Color(28, 28, 28);

        if (selectedSquare != nullptr && selectedSquare->getId() == square.getId())
        {
            outlineThickness = 4.0f;
            outline = sf::Color(255, 215, 0);
        }

        drawQuad(cellQuad(square.getId()), fill, outlineThickness, outline);
    }

    // Contour externe plus proche d’un vrai plateau polygonal.
    sf::VertexArray border(sf::PrimitiveType::LineStrip, 9);
    border[0].position = {kGridX[0][0],  kGridY[0]};
    border[1].position = {kGridX[0][12], kGridY[0]};
    border[2].position = {kGridX[2][12], kGridY[2]};
    border[3].position = {kGridX[4][12], kGridY[4]};
    border[4].position = {kGridX[8][12], kGridY[8]};
    border[5].position = {kGridX[8][0],  kGridY[8]};
    border[6].position = {kGridX[4][0],  kGridY[4]};
    border[7].position = {kGridX[2][0],  kGridY[2]};
    border[8].position = border[0].position;

    for (std::size_t i = 0; i < border.getVertexCount(); ++i)
    {
        border[i].color = sf::Color(220, 220, 220);
    }

    m_window.draw(border);

    if (hasFont())
    {
        drawText("12", {897.f, 206.f}, 16, sf::Color::White, true);
        drawText("11", {883.f, 160.f}, 16, sf::Color::White, true);
        drawText("10", {864.f, 118.f}, 16, sf::Color::White, true);
        drawText("9",  {838.f,  84.f}, 16, sf::Color::White, true);

        drawText("8", {139.f,  84.f}, 16, sf::Color::White, true);
        drawText("7", {122.f, 118.f}, 16, sf::Color::White, true);
        drawText("6", {103.f, 160.f}, 16, sf::Color::White, true);
        drawText("5", { 90.f, 206.f}, 16, sf::Color::White, true);

        drawText("a", {236.f, 760.f}, 18, sf::Color::White, true);
        drawText("b", {294.f, 760.f}, 18, sf::Color::White, true);
        drawText("c", {352.f, 760.f}, 18, sf::Color::White, true);
        drawText("d", {410.f, 760.f}, 18, sf::Color::White, true);
        drawText("e", {468.f, 760.f}, 18, sf::Color::White, true);
        drawText("f", {526.f, 760.f}, 18, sf::Color::White, true);
        drawText("g", {584.f, 760.f}, 18, sf::Color::White, true);
        drawText("h", {642.f, 760.f}, 18, sf::Color::White, true);
    }
}

void SFMLView::drawHighlights(const Game& game, const std::vector<Move>& highlightedMoves)
{
    const Board& board = game.getBoard();

    for (const Move& move : highlightedMoves)
    {
        if (!move.isValid())
        {
            continue;
        }

        const int targetId = move.getToId();
        if (!board.isActiveSquare(targetId))
        {
            continue;
        }

        sf::Color fill;

        if (move.getType() == MoveType::Capture || move.getType() == MoveType::EnPassant)
        {
            fill = withAlpha(sf::Color::Red, 120);
        }
        else if (move.getType() == MoveType::Castling || move.getType() == MoveType::Promotion)
        {
            fill = withAlpha(sf::Color(180, 90, 255), 115);
        }
        else
        {
            fill = withAlpha(sf::Color::Green, 105);
        }

        drawQuad(cellQuad(targetId), fill, 2.0f, sf::Color::White);
    }
}

void SFMLView::drawPieces(const Game& game)
{
    const Board& board = game.getBoard();

    for (const Square& square : board.getSquares())
    {
        if (!board.isActiveSquare(square.getId()) || !square.isOccupied())
        {
            continue;
        }

        Piece* piece = square.getPiece();
        if (piece == nullptr || piece->isCaptured())
        {
            continue;
        }

        drawPiece(*piece, cellCenter(square.getId()), 18.f);
    }
}

void SFMLView::drawPiece(const Piece& piece, sf::Vector2f center, float radius)
{
    const sf::Color baseColor = colorForPlayer(piece.getOwner()->getColor());

const bool darkPiece =
    piece.getOwner()->getColor() == PlayerColor::Blue;

const sf::Color markerColor = darkPiece
    ? sf::Color(245, 245, 245)
    : sf::Color(20, 20, 20);

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
    sf::RectangleShape panel({kRightPanelWidth, kRightPanelHeight});
    panel.setPosition({kRightPanelX, kRightPanelY});
    panel.setFillColor(sf::Color(36, 42, 55));
    panel.setOutlineThickness(2.f);
    panel.setOutlineColor(sf::Color(180, 180, 180));
    m_window.draw(panel);

    const Player* currentPlayer = game.getCurrentPlayer();
    const sf::Color turnColor = (currentPlayer != nullptr)
        ? colorForPlayer(currentPlayer->getColor())
        : sf::Color(120, 120, 120);

    sf::RectangleShape turnBanner({300.f, 45.f});
    turnBanner.setPosition({1170.f, 94.f});
    turnBanner.setFillColor(turnColor);
    m_window.draw(turnBanner);

    const PlayerColor colors[3] = {
        PlayerColor::Red,
        PlayerColor::Blue,
        PlayerColor::Green
    };

    for (int i = 0; i < 3; ++i)
    {
        sf::CircleShape indicator(28.f);
        indicator.setOrigin({28.f, 28.f});
        indicator.setPosition({
            1210.f + (i * 85.f),
            190.f
        });
        indicator.setFillColor(colorForPlayer(colors[i]));
        indicator.setOutlineThickness(4.f);

        const Player* player = game.getPlayer(colors[i]);
        const bool eliminated = (player == nullptr) ? true : player->isEliminated();

        if (eliminated)
        {
            indicator.setFillColor(sf::Color(80, 80, 80));
            indicator.setOutlineColor(sf::Color(40, 40, 40));
        }
        else if (currentPlayer != nullptr && currentPlayer->getColor() == colors[i])
        {
            indicator.setOutlineColor(sf::Color::White);
        }
        else
        {
            indicator.setOutlineColor(sf::Color(70, 70, 70));
        }

        m_window.draw(indicator);
    }

    for (int i = 0; i < 4; ++i)
    {
        sf::RectangleShape infoBox({300.f, 88.f});
        infoBox.setPosition({
            1170.f,
            255.f + (i * 102.f)
        });
        infoBox.setFillColor((i % 2 == 0) ? sf::Color(60, 68, 82) : sf::Color(52, 60, 74));
        infoBox.setOutlineThickness(1.f);
        infoBox.setOutlineColor(sf::Color(100, 100, 100));
        m_window.draw(infoBox);
    }

    if (selectedSquare != nullptr)
    {
        sf::RectangleShape marker({84.f, 84.f});
        marker.setPosition({1278.f, 612.f});
        marker.setFillColor(sf::Color(185, 185, 185));
        marker.setOutlineThickness(4.f);
        marker.setOutlineColor(sf::Color(255, 215, 0));
        m_window.draw(marker);

        if (selectedSquare->isOccupied() && selectedSquare->getPiece() != nullptr)
        {
            drawPiece(*selectedSquare->getPiece(), {1320.f, 654.f}, 22.f);
        }
    }

    if (!hasFont())
    {
        return;
    }

    drawText("YALTA 3 JOUEURS", {1180.f, 74.f}, 24, sf::Color::White, true);

    drawText(
        "Etat : " + std::string(toString(game.getState())),
        {1184.f, 268.f},
        18,
        stateColor(game.getState()),
        true);

    std::string currentText = "Joueur : none";
    if (currentPlayer != nullptr)
    {
        currentText = "Joueur : " + std::string(toString(currentPlayer->getColor()));
    }
    drawText(currentText, {1184.f, 296.f}, 18, sf::Color::White);

    std::string selectedText = "Selection : aucune";
    if (selectedSquare != nullptr)
    {
        selectedText = "Selection : " + game.getBoard().getLabel(selectedSquare->getId())
                     + " (#" + std::to_string(selectedSquare->getId()) + ")";
    }
    drawText(selectedText, {1184.f, 370.f}, 17, sf::Color::White);

    std::string winnerText = "Vainqueur : aucun";
    if (game.getWinner() != PlayerColor::None)
    {
        winnerText = "Vainqueur : " + std::string(toString(game.getWinner()));
    }
    drawText(winnerText, {1184.f, 398.f}, 17, sf::Color::White);

    drawText("Dernier evenement :", {1184.f, 472.f}, 17, sf::Color::White, true);
    drawText(truncateText(game.getLastEvent(), 70), {1184.f, 500.f}, 16, sf::Color(225, 225, 225));

    drawText("Commandes :", {1184.f, 574.f}, 17, sf::Color::White, true);
    drawText("Clic gauche  : selection / coup", {1184.f, 602.f}, 15, sf::Color(220, 220, 220));
    drawText("Clic droit   : annuler selection", {1184.f, 624.f}, 15, sf::Color(220, 220, 220));
    drawText("Esc          : quitter", {1184.f, 646.f}, 15, sf::Color(220, 220, 220));
    drawText("R            : fermer si partie finie", {1184.f, 668.f}, 15, sf::Color(220, 220, 220));
}

void SFMLView::drawEndgameOverlay(const Game& game)
{
    sf::RectangleShape overlay({930.f, 180.f});
    overlay.setPosition({18.f, 370.f});
    overlay.setFillColor(sf::Color(20, 22, 28, 210));
    overlay.setOutlineThickness(3.f);
    overlay.setOutlineColor(stateColor(game.getState()));
    m_window.draw(overlay);

    if (!hasFont())
    {
        return;
    }

    std::string title = "PARTIE TERMINEE";
    if (game.getState() == GameState::Draw)
    {
        title = "MATCH NUL";
    }
    else if (game.getState() == GameState::Stalemate)
    {
        title = "PAT / NULLE";
    }
    else if (game.getState() == GameState::WinByElimination)
    {
        title = "VICTOIRE";
    }

    drawText(title, {290.f, 395.f}, 34, stateColor(game.getState()), true);

    std::string detail = truncateText(game.getLastEvent(), 95);
    drawText(detail, {80.f, 450.f}, 20, sf::Color::White);

    if (game.getWinner() != PlayerColor::None)
    {
        drawText(
            "Vainqueur : " + std::string(toString(game.getWinner())),
            {320.f, 490.f},
            22,
            colorForPlayer(game.getWinner()),
            true);
    }
}

sf::Color SFMLView::colorForPlayer(PlayerColor color) const
{
    switch (color)
    {
        case PlayerColor::Red:
            return sf::Color(180, 32, 45);
        case PlayerColor::Blue:
            return sf::Color(25, 25, 25);
        case PlayerColor::Green:
            return sf::Color(235, 235, 235);
        default:
            return sf::Color(150, 150, 150);
    }
}

sf::Color SFMLView::squareColor(int row, int col) const
{
    const bool dark = ((row + col) % 2 == 0);
    return dark ? sf::Color(194, 208, 224) : sf::Color(225, 227, 234);
}

sf::Color SFMLView::stateColor(GameState state) const
{
    switch (state)
    {
        case GameState::Check:
            return sf::Color(255, 215, 0);
        case GameState::WinByElimination:
            return sf::Color(120, 235, 120);
        case GameState::Draw:
        case GameState::Stalemate:
            return sf::Color(180, 180, 255);
        case GameState::Ongoing:
        default:
            return sf::Color::White;
    }
}

SFMLView::Quad SFMLView::cellQuad(int squareId) const
{
    const int row = squareId / 12;
    const int col = squareId % 12;

    return {{
        {kGridX[row][col],     kGridY[row]},
        {kGridX[row][col + 1], kGridY[row]},
        {kGridX[row + 1][col + 1], kGridY[row + 1]},
        {kGridX[row + 1][col],     kGridY[row + 1]}
    }};
}

sf::Vector2f SFMLView::cellCenter(int squareId) const
{
    const Quad quad = cellQuad(squareId);

    sf::Vector2f center(0.f, 0.f);
    for (const sf::Vector2f& p : quad)
    {
        center += p;
    }

    return center / 4.f;
}

bool SFMLView::pointInTriangle(sf::Vector2f p, sf::Vector2f a, sf::Vector2f b, sf::Vector2f c) const
{
    const float c1 = cross(a, b, p);
    const float c2 = cross(b, c, p);
    const float c3 = cross(c, a, p);

    const bool hasNeg = (c1 < 0.f) || (c2 < 0.f) || (c3 < 0.f);
    const bool hasPos = (c1 > 0.f) || (c2 > 0.f) || (c3 > 0.f);

    return !(hasNeg && hasPos);
}

bool SFMLView::pointInQuad(sf::Vector2f point, const Quad& quad) const
{
    return pointInTriangle(point, quad[0], quad[1], quad[2])
        || pointInTriangle(point, quad[0], quad[2], quad[3]);
}

void SFMLView::drawQuad(
    const Quad& quad,
    sf::Color fillColor,
    float outlineThickness,
    sf::Color outlineColor)
{
    sf::ConvexShape shape;
    shape.setPointCount(4);
    shape.setPoint(0, quad[0]);
    shape.setPoint(1, quad[1]);
    shape.setPoint(2, quad[2]);
    shape.setPoint(3, quad[3]);
    shape.setFillColor(fillColor);
    shape.setOutlineThickness(outlineThickness);
    shape.setOutlineColor(outlineColor);
    m_window.draw(shape);
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

void SFMLView::tryLoadFont()
{
    const std::vector<std::string> candidates = {
        "/System/Library/Fonts/Supplemental/Arial Unicode.ttf",
        "/System/Library/Fonts/Supplemental/Arial.ttf",
        "/System/Library/Fonts/Supplemental/Helvetica.ttc",
        "/Library/Fonts/Arial.ttf"
    };

    for (const std::string& path : candidates)
    {
        std::unique_ptr<sf::Font> font = std::make_unique<sf::Font>();
        if (font->openFromFile(path))
        {
            m_font = std::move(font);
            return;
        }
    }
}

bool SFMLView::hasFont() const
{
    return m_font != nullptr;
}

void SFMLView::drawText(
    const std::string& text,
    sf::Vector2f position,
    unsigned int size,
    sf::Color color,
    bool bold)
{
    if (!hasFont())
    {
        return;
    }

    sf::Text drawable(*m_font);
    drawable.setString(text);
    drawable.setCharacterSize(size);
    drawable.setFillColor(color);
    drawable.setPosition(position);

    if (bold)
    {
        drawable.setStyle(sf::Text::Bold);
    }

    m_window.draw(drawable);
}

std::string SFMLView::truncateText(const std::string& text, std::size_t maxLen) const
{
    if (text.size() <= maxLen)
    {
        return text;
    }

    if (maxLen < 4)
    {
        return text.substr(0, maxLen);
    }

    return text.substr(0, maxLen - 3) + "...";
}