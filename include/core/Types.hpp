#pragma once

#include <string>

// Représentation actuelle du projet :
// - plateau de 96 cases modélisé comme une grille 8 x 12
// - 3 joueurs : Red, Blue, Green
// Certaines règles Yalta exactes devront être ajustées plus tard
// si le diagramme UML et la topologie finale du plateau évoluent.

enum class PlayerColor
{
    Red,
    Blue,
    Green,
    None
};

enum class PieceType
{
    King,
    Queen,
    Rook,
    Bishop,
    Knight,
    Pawn,
    None
};

enum class MoveType
{
    Normal,
    Capture,
    Castling,
    EnPassant,
    Promotion
};

enum class GameState
{
    Ongoing,
    Check,
    Stalemate,
    WinByElimination,
    Draw
};

inline std::string toString(PlayerColor color)
{
    switch (color)
    {
        case PlayerColor::Red: return "Red";
        case PlayerColor::Blue: return "Blue";
        case PlayerColor::Green: return "Green";
        default: return "None";
    }
}

inline std::string toString(PieceType type)
{
    switch (type)
    {
        case PieceType::King: return "King";
        case PieceType::Queen: return "Queen";
        case PieceType::Rook: return "Rook";
        case PieceType::Bishop: return "Bishop";
        case PieceType::Knight: return "Knight";
        case PieceType::Pawn: return "Pawn";
        default: return "None";
    }
}

inline std::string toString(MoveType type)
{
    switch (type)
    {
        case MoveType::Normal: return "Normal";
        case MoveType::Capture: return "Capture";
        case MoveType::Castling: return "Castling";
        case MoveType::EnPassant: return "EnPassant";
        case MoveType::Promotion: return "Promotion";
        default: return "UnknownMoveType";
    }
}

inline std::string toString(GameState state)
{
    switch (state)
    {
        case GameState::Ongoing: return "Ongoing";
        case GameState::Check: return "Check";
        case GameState::Stalemate: return "Stalemate";
        case GameState::WinByElimination: return "WinByElimination";
        case GameState::Draw: return "Draw";
        default: return "UnknownGameState";
    }
}

inline char toPieceLetter(PieceType type)
{
    switch (type)
    {
        case PieceType::King: return 'K';
        case PieceType::Queen: return 'Q';
        case PieceType::Rook: return 'R';
        case PieceType::Bishop: return 'B';
        case PieceType::Knight: return 'N';
        case PieceType::Pawn: return 'P';
        default: return '.';
    }
}

inline char toPlayerLetter(PlayerColor color)
{
    switch (color)
    {
        case PlayerColor::Red: return 'R';
        case PlayerColor::Blue: return 'B';
        case PlayerColor::Green: return 'G';
        default: return '.';
    }
}