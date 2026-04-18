#pragma once

#include <SFML/System/Clock.hpp>
#include <vector>

#include "ai/AlphaBetaAI.hpp"
#include "core/Game.hpp"
#include "core/Move.hpp"

class SFMLView;
class Square;

enum class PlayerControlMode
{
    Human,
    AI
};

struct SFMLControllerConfig
{
    PlayerControlMode redMode = PlayerControlMode::Human;
    PlayerControlMode blueMode = PlayerControlMode::AI;
    PlayerControlMode greenMode = PlayerControlMode::AI;

    int aiDepth = 3;
    unsigned int aiThreadCount = 0;
};

class SFMLGameController
{
public:
    SFMLGameController(Game& game, SFMLView& view, const SFMLControllerConfig& config);

    void setupDefaultPosition();
    void run();

private:
    void placeHorizontalArmy(PlayerColor color, int backRow, int pawnRow);
    void placeVerticalArmy(PlayerColor color, int backCol, int pawnCol);

    void resetSelection();
    void selectSquare(Square* square);
    void handleBoardClick(int squareId);
    bool tryPlayMoveTo(int targetSquareId);

    bool isHumanTurn() const;
    bool isAIControlled(PlayerColor color) const;
    AlphaBetaAI* getAIFor(PlayerColor color);

    void processAITurn();

    bool isSessionFinished() const;
    void updateWindowTitle() const;

    Game& m_game;
    SFMLView& m_view;
    SFMLControllerConfig m_config;

    Square* m_selectedSquare;
    std::vector<Move> m_selectedMoves;

    AlphaBetaAI m_redAI;
    AlphaBetaAI m_blueAI;
    AlphaBetaAI m_greenAI;
    sf::Clock m_aiClock;
};