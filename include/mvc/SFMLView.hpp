#pragma once

#include <SFML/Graphics.hpp>
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

    int squareIdFromPixel(sf::Vector2i pixel) const;
    void render(
        const Game& game,
        const Square* selectedSquare,
        const std::vector<Move>& highlightedMoves);

private:
    void drawBoard(const Game& game, const Square* selectedSquare);
    void drawHighlights(const Game& game, const std::vector<Move>& highlightedMoves);
    void drawPieces(const Game& game);
    void drawPiece(const Piece& piece, sf::Vector2f center, float radius);
    void drawStatusPanel(const Game& game, const Square* selectedSquare);

    sf::Color colorForPlayer(PlayerColor color) const;
    sf::Color squareColor(int row, int col) const;

    sf::Vector2f cellTopLeft(int row, int col) const;
    sf::Vector2f cellCenter(int row, int col) const;

    void drawLine(
        sf::Vector2f start,
        sf::Vector2f end,
        float thickness,
        sf::Color color);

    sf::RenderWindow m_window;

    static constexpr unsigned int kWindowWidth = 1280;
    static constexpr unsigned int kWindowHeight = 860;

    static constexpr float kBoardOffsetX = 40.f;
    static constexpr float kBoardOffsetY = 40.f;
    static constexpr float kCellSize = 64.f;

    static constexpr float kBoardWidth = 12.f * kCellSize;
    static constexpr float kBoardHeight = 8.f * kCellSize;
};