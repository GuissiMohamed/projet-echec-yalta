#pragma once

#include <SFML/Graphics.hpp>
#include <array>
#include <memory>
#include <string>
#include <vector>

#include "core/Move.hpp"
#include "core/Types.hpp"

class Game;
class Square;
class Piece;

class SFMLView
{
public:
    SFMLView();

    sf::RenderWindow& getWindow();

    int squareIdFromPixel(const Game& game, sf::Vector2i pixel) const;
    void render(
        const Game& game,
        const Square* selectedSquare,
        const std::vector<Move>& highlightedMoves);

private:
    using Quad = std::array<sf::Vector2f, 4>;

    void drawBoard(const Game& game, const Square* selectedSquare);
    void drawHighlights(const Game& game, const std::vector<Move>& highlightedMoves);
    void drawPieces(const Game& game);
    void drawPiece(const Piece& piece, sf::Vector2f center, float radius);
    void drawStatusPanel(const Game& game, const Square* selectedSquare);
    void drawEndgameOverlay(const Game& game);

    sf::Color colorForPlayer(PlayerColor color) const;
    sf::Color squareColor(int row, int col) const;
    sf::Color stateColor(GameState state) const;

    Quad cellQuad(int squareId) const;
    sf::Vector2f cellCenter(int squareId) const;

    bool pointInQuad(sf::Vector2f point, const Quad& quad) const;
    bool pointInTriangle(sf::Vector2f p, sf::Vector2f a, sf::Vector2f b, sf::Vector2f c) const;

    void drawQuad(
        const Quad& quad,
        sf::Color fillColor,
        float outlineThickness = 1.0f,
        sf::Color outlineColor = sf::Color(45, 45, 45));

    void drawLine(
        sf::Vector2f start,
        sf::Vector2f end,
        float thickness,
        sf::Color color);

    void tryLoadFont();
    bool hasFont() const;
    void drawText(
        const std::string& text,
        sf::Vector2f position,
        unsigned int size,
        sf::Color color,
        bool bold = false);
    std::string truncateText(const std::string& text, std::size_t maxLen) const;

    sf::RenderWindow m_window;
    std::unique_ptr<sf::Font> m_font;

    static constexpr unsigned int kWindowWidth = 1536;
    static constexpr unsigned int kWindowHeight = 960;

    static constexpr float kRightPanelX = 1138.f;
    static constexpr float kRightPanelY = 66.f;
    static constexpr float kRightPanelWidth = 362.f;
    static constexpr float kRightPanelHeight = 694.f;
};