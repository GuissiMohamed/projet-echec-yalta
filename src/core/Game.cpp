#include "core/Game.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <iomanip>
#include <memory>
#include <sstream>

#include "core/Piece.hpp"
#include "core/Square.hpp"

namespace
{
    std::string formatMoveForMessage(const Move& move)
    {
        if (!move.isValid())
        {
            return "invalid move";
        }

        std::ostringstream stream;
        stream << move.getFromId() << " -> " << move.getToId()
               << " [" << toString(move.getType()) << "]";

        if (move.getType() == MoveType::Promotion)
        {
            stream << " => " << toString(move.getPromotionType());
        }

        return stream.str();
    }

    int diagonalTargetId(const Board& board, int row, int col, DiagonalDirection direction)
    {
        switch (direction)
        {
            case DiagonalDirection::UpLeft:
                return board.toIndex(row - 1, col - 1);
            case DiagonalDirection::UpRight:
                return board.toIndex(row - 1, col + 1);
            case DiagonalDirection::DownLeft:
                return board.toIndex(row + 1, col - 1);
            case DiagonalDirection::DownRight:
                return board.toIndex(row + 1, col + 1);
            default:
                return -1;
        }
    }

    bool matchesDiagonalDelta(int rowDelta, int colDelta, DiagonalDirection direction)
    {
        switch (direction)
        {
            case DiagonalDirection::UpLeft:
                return rowDelta == -1 && colDelta == -1;
            case DiagonalDirection::UpRight:
                return rowDelta == -1 && colDelta == 1;
            case DiagonalDirection::DownLeft:
                return rowDelta == 1 && colDelta == -1;
            case DiagonalDirection::DownRight:
                return rowDelta == 1 && colDelta == 1;
            default:
                return false;
        }
    }
}

Game::Game()
    : m_board(),
      m_currentPlayerIndex(0),
      m_state(GameState::Ongoing),
      m_winner(PlayerColor::None),
      m_lastEvent("Initialisation du moteur Yalta (étape 5)."),
      m_enPassantTargetSquareId(-1),
      m_enPassantVictimSquareId(-1),
      m_enPassantRemainingResponses(0)
{
    m_players.emplace_back(std::make_unique<Player>("Alice", PlayerColor::Red));
    m_players.emplace_back(std::make_unique<Player>("Bob", PlayerColor::Blue));
    m_players.emplace_back(std::make_unique<Player>("Charlie", PlayerColor::Green));
}

Game::Game(const Game& other)
    : m_board(other.getBoard().cloneTopology()),
      m_currentPlayerIndex(other.m_currentPlayerIndex),
      m_state(other.m_state),
      m_winner(other.m_winner),
      m_lastEvent(other.m_lastEvent),
      m_enPassantTargetSquareId(other.m_enPassantTargetSquareId),
      m_enPassantVictimSquareId(other.m_enPassantVictimSquareId),
      m_enPassantRemainingResponses(other.m_enPassantRemainingResponses)
{
    for (const auto& player : other.m_players)
    {
        m_players.emplace_back(std::make_unique<Player>(player->getName(), player->getColor()));
        m_players.back()->setEliminated(player->isEliminated());
    }

    for (const auto& piece : other.m_pieces)
    {
        if (piece == nullptr)
        {
            continue;
        }

        Player* newOwner = getPlayer(piece->getOwner()->getColor());
        Square* newSquare = nullptr;

        if (piece->getSquare() != nullptr)
        {
            newSquare = m_board.getSquare(piece->getSquare()->getId());
        }

        m_pieces.emplace_back(std::make_unique<Piece>(
            piece->getType(),
            newOwner,
            newSquare,
            &m_board));

        Piece* newPiece = m_pieces.back().get();
        newPiece->setHasMoved(piece->hasMoved());
        newPiece->setCaptured(piece->isCaptured());

        if (piece->getSquare() == nullptr)
        {
            newPiece->setSquare(nullptr);
        }

        if (newSquare != nullptr && !piece->isCaptured())
        {
            newSquare->setPiece(newPiece);
        }

        if (newOwner != nullptr)
        {
            newOwner->addPiece(newPiece);
        }
    }
}

Game::~Game() = default;

Board& Game::getBoard()
{
    return m_board;
}

const Board& Game::getBoard() const
{
    return m_board;
}

Player* Game::getPlayer(PlayerColor color)
{
    for (const auto& player : m_players)
    {
        if (player->getColor() == color)
        {
            return player.get();
        }
    }

    return nullptr;
}

const Player* Game::getPlayer(PlayerColor color) const
{
    for (const auto& player : m_players)
    {
        if (player->getColor() == color)
        {
            return player.get();
        }
    }

    return nullptr;
}

Player* Game::getCurrentPlayer()
{
    if (m_players.empty())
    {
        return nullptr;
    }

    return m_players[m_currentPlayerIndex].get();
}

const Player* Game::getCurrentPlayer() const
{
    if (m_players.empty())
    {
        return nullptr;
    }

    return m_players[m_currentPlayerIndex].get();
}

void Game::setCurrentPlayer(PlayerColor color)
{
    for (std::size_t i = 0; i < m_players.size(); ++i)
    {
        if (m_players[i]->getColor() == color)
        {
            m_currentPlayerIndex = i;
            return;
        }
    }
}

Piece* Game::addPiece(PieceType type, PlayerColor color, int squareId)
{
    Player* owner = getPlayer(color);
    Square* square = m_board.getSquare(squareId);

    if (owner == nullptr || square == nullptr || square->isOccupied())
    {
        return nullptr;
    }

    m_pieces.emplace_back(std::make_unique<Piece>(type, owner, square, &m_board));
    Piece* piece = m_pieces.back().get();

    square->setPiece(piece);
    owner->addPiece(piece);

    return piece;
}

Piece* Game::getKing(PlayerColor color) const
{
    const Player* player = getPlayer(color);
    if (player == nullptr)
    {
        return nullptr;
    }

    for (Piece* piece : player->getPieces())
    {
        if (piece != nullptr
            && !piece->isCaptured()
            && piece->getType() == PieceType::King)
        {
            return piece;
        }
    }

    return nullptr;
}

std::vector<Move> Game::getCandidateMovesForPiece(const Piece& piece)
{
    std::vector<Move> moves = piece.getPseudoLegalMoves();
    const std::vector<Move> specialMoves = getSpecialMovesForPiece(piece);
    moves.insert(moves.end(), specialMoves.begin(), specialMoves.end());

    std::vector<Move> decoratedMoves;
    decoratedMoves.reserve(moves.size() + 8);

    for (const Move& move : moves)
    {
        if (!move.isValid())
        {
            continue;
        }

        if (wouldCaptureEnemyKing(move, piece.getOwner()->getColor()))
        {
            continue;
        }

        const Square* toSquare = move.resolveTo(m_board);
        if (toSquare == nullptr)
        {
            continue;
        }

        if (piece.getType() == PieceType::Pawn
            && move.getType() != MoveType::EnPassant
            && isPromotionSquare(piece.getOwner()->getColor(), *toSquare))
        {
            decoratedMoves.emplace_back(move.getFromId(), move.getToId(), MoveType::Promotion, PieceType::Queen);
            decoratedMoves.emplace_back(move.getFromId(), move.getToId(), MoveType::Promotion, PieceType::Rook);
            decoratedMoves.emplace_back(move.getFromId(), move.getToId(), MoveType::Promotion, PieceType::Bishop);
            decoratedMoves.emplace_back(move.getFromId(), move.getToId(), MoveType::Promotion, PieceType::Knight);
            continue;
        }

        decoratedMoves.push_back(move);
    }

    return decoratedMoves;
}

std::vector<Move> Game::getSpecialMovesForPiece(const Piece& piece)
{
    std::vector<Move> moves;

    if (piece.isCaptured() || piece.getSquare() == nullptr || piece.getOwner() == nullptr)
    {
        return moves;
    }

    if (piece.getType() == PieceType::King && !piece.hasMoved())
    {
        if (!isPlayerInCheck(piece.getOwner()->getColor()))
        {
            const int row = m_board.getRow(piece.getSquare()->getId());
            const int col = m_board.getCol(piece.getSquare()->getId());

            for (const int direction : {-1, 1})
            {
                int scanCol = col + direction;
                Piece* rook = nullptr;

                while (m_board.isValid(row, scanCol))
                {
                    Square* square = m_board.getSquare(row, scanCol);
                    if (square == nullptr)
                    {
                        break;
                    }

                    if (square->isOccupied())
                    {
                        Piece* occupiedPiece = square->getPiece();
                        if (occupiedPiece != nullptr
                            && occupiedPiece->getOwner() == piece.getOwner()
                            && occupiedPiece->getType() == PieceType::Rook
                            && !occupiedPiece->hasMoved())
                        {
                            rook = occupiedPiece;
                        }
                        break;
                    }

                    scanCol += direction;
                }

                if (rook == nullptr || rook->getSquare() == nullptr)
                {
                    continue;
                }

                const int rookCol = m_board.getCol(rook->getSquare()->getId());
                if (std::abs(rookCol - col) < 3)
                {
                    continue;
                }

                bool pathClear = true;
                for (int c = col + direction; c != rookCol; c += direction)
                {
                    Square* between = m_board.getSquare(row, c);
                    if (between != nullptr && between->isOccupied())
                    {
                        pathClear = false;
                        break;
                    }
                }

                if (!pathClear)
                {
                    continue;
                }

                Square* firstKingSquare = m_board.getSquare(row, col + direction);
                Square* secondKingSquare = m_board.getSquare(row, col + (2 * direction));

                if (firstKingSquare == nullptr || secondKingSquare == nullptr)
                {
                    continue;
                }

                if (firstKingSquare->isOccupied() || secondKingSquare->isOccupied())
                {
                    continue;
                }

                if (isSquareAttackedByOpponents(*firstKingSquare, piece.getOwner()->getColor())
                    || isSquareAttackedByOpponents(*secondKingSquare, piece.getOwner()->getColor()))
                {
                    continue;
                }

                moves.emplace_back(piece.getSquare()->getId(), secondKingSquare->getId(), MoveType::Castling);
            }
        }
    }

    if (piece.getType() == PieceType::Pawn
        && m_enPassantTargetSquareId >= 0
        && m_enPassantVictimSquareId >= 0
        && m_enPassantRemainingResponses > 0)
    {
        Square* targetSquare = m_board.getSquare(m_enPassantTargetSquareId);
        Square* victimSquare = m_board.getSquare(m_enPassantVictimSquareId);

        if (targetSquare != nullptr && victimSquare != nullptr && victimSquare->isOccupied())
        {
            Piece* victim = victimSquare->getPiece();
            if (victim != nullptr
                && !victim->isCaptured()
                && victim->getType() == PieceType::Pawn
                && victim->getOwner() != piece.getOwner())
            {
                const int currentRow = m_board.getRow(piece.getSquare()->getId());
                const int currentCol = m_board.getCol(piece.getSquare()->getId());

                const auto [captureA, captureB] = piece.getOwner()->getPawnCaptureDirections();
                const std::array<DiagonalDirection, 2> captureDirections = {captureA, captureB};

                for (DiagonalDirection direction : captureDirections)
                {
                    const int targetId = diagonalTargetId(m_board, currentRow, currentCol, direction);
                    if (targetId < 0)
                    {
                        continue;
                    }

                    Square* candidateSquare = m_board.getSquare(targetId);
                    if (candidateSquare == targetSquare && !candidateSquare->isOccupied())
                    {
                        moves.emplace_back(piece.getSquare()->getId(), candidateSquare->getId(), MoveType::EnPassant);
                    }
                }
            }
        }
    }

    return moves;
}

std::vector<Move> Game::getLegalMovesForPiece(const Piece& piece)
{
    std::vector<Move> legalMoves;

    if (piece.isCaptured() || piece.getOwner() == nullptr || piece.getOwner()->isEliminated())
    {
        return legalMoves;
    }

    const std::vector<Move> candidateMoves = getCandidateMovesForPiece(piece);

    for (const Move& move : candidateMoves)
    {
        MoveStateSnapshot snapshot;
        if (!applyMoveInternal(move, snapshot))
        {
            continue;
        }

        const bool leavesOwnKingInCheck = isPlayerInCheck(piece.getOwner()->getColor());
        undoMoveInternal(snapshot);

        if (!leavesOwnKingInCheck)
        {
            legalMoves.push_back(move);
        }
    }

    return legalMoves;
}

std::vector<Move> Game::getLegalMovesForPlayer(PlayerColor color)
{
    std::vector<Move> legalMoves;

    Player* player = getPlayer(color);
    if (player == nullptr || player->isEliminated())
    {
        return legalMoves;
    }

    for (Piece* piece : player->getPieces())
    {
        if (piece == nullptr || piece->isCaptured())
        {
            continue;
        }

        const std::vector<Move> pieceMoves = getLegalMovesForPiece(*piece);
        legalMoves.insert(legalMoves.end(), pieceMoves.begin(), pieceMoves.end());
    }

    return legalMoves;
}

bool Game::doesPieceAttackSquare(const Piece& piece, const Square& target) const
{
    if (piece.isCaptured() || piece.getSquare() == nullptr || piece.getOwner() == nullptr)
    {
        return false;
    }

    const int fromRow = m_board.getRow(piece.getSquare()->getId());
    const int fromCol = m_board.getCol(piece.getSquare()->getId());
    const int toRow = m_board.getRow(target.getId());
    const int toCol = m_board.getCol(target.getId());

    const int rowDelta = toRow - fromRow;
    const int colDelta = toCol - fromCol;
    const int absRowDelta = std::abs(rowDelta);
    const int absColDelta = std::abs(colDelta);

    if (piece.getType() == PieceType::King)
    {
        return absRowDelta <= 1 && absColDelta <= 1 && !(rowDelta == 0 && colDelta == 0);
    }

    if (piece.getType() == PieceType::Knight)
    {
        return (absRowDelta == 2 && absColDelta == 1) || (absRowDelta == 1 && absColDelta == 2);
    }

    if (piece.getType() == PieceType::Pawn)
    {
        const auto [captureA, captureB] = piece.getOwner()->getPawnCaptureDirections();

        if (matchesDiagonalDelta(rowDelta, colDelta, captureA)
            || matchesDiagonalDelta(rowDelta, colDelta, captureB))
        {
            return true;
        }

        return false;
    }

    auto pathClear = [this, fromRow, fromCol, toRow, toCol]() -> bool
    {
        const int stepRow = (toRow > fromRow) ? 1 : ((toRow < fromRow) ? -1 : 0);
        const int stepCol = (toCol > fromCol) ? 1 : ((toCol < fromCol) ? -1 : 0);

        int currentRow = fromRow + stepRow;
        int currentCol = fromCol + stepCol;

        while (currentRow != toRow || currentCol != toCol)
        {
            const Square* between = m_board.getSquare(currentRow, currentCol);
            if (between != nullptr && between->isOccupied())
            {
                return false;
            }

            currentRow += stepRow;
            currentCol += stepCol;
        }

        return true;
    };

    if (piece.getType() == PieceType::Rook)
    {
        if (fromRow != toRow && fromCol != toCol)
        {
            return false;
        }

        return pathClear();
    }

    if (piece.getType() == PieceType::Bishop)
    {
        if (absRowDelta != absColDelta)
        {
            return false;
        }

        return pathClear();
    }

    if (piece.getType() == PieceType::Queen)
    {
        const bool rookLike = (fromRow == toRow || fromCol == toCol);
        const bool bishopLike = (absRowDelta == absColDelta);
        if (!rookLike && !bishopLike)
        {
            return false;
        }

        return pathClear();
    }

    return false;
}

bool Game::isSquareAttackedByOpponents(const Square& square, PlayerColor defenderColor) const
{
    for (const auto& player : m_players)
    {
        if (player->getColor() == defenderColor || player->isEliminated())
        {
            continue;
        }

        for (Piece* piece : player->getPieces())
        {
            if (piece != nullptr && !piece->isCaptured() && doesPieceAttackSquare(*piece, square))
            {
                return true;
            }
        }
    }

    return false;
}

bool Game::isPlayerInCheck(PlayerColor color) const
{
    const Player* player = getPlayer(color);
    if (player == nullptr || player->isEliminated())
    {
        return false;
    }

    Piece* king = getKing(color);
    if (king == nullptr || king->getSquare() == nullptr)
    {
        return true;
    }

    return isSquareAttackedByOpponents(*king->getSquare(), color);
}

bool Game::hasAnyLegalMove(PlayerColor color)
{
    return !getLegalMovesForPlayer(color).empty();
}

bool Game::isPlayerCheckmated(PlayerColor color)
{
    const Player* player = getPlayer(color);
    if (player == nullptr || player->isEliminated())
    {
        return false;
    }

    return isPlayerInCheck(color) && !hasAnyLegalMove(color);
}

bool Game::isPlayerStalemated(PlayerColor color)
{
    const Player* player = getPlayer(color);
    if (player == nullptr || player->isEliminated())
    {
        return false;
    }

    return !isPlayerInCheck(color) && !hasAnyLegalMove(color);
}

bool Game::isPromotionSquare(PlayerColor color, const Square& square) const
{
    const int row = m_board.getRow(square.getId());
    const int col = m_board.getCol(square.getId());

    switch (color)
    {
        case PlayerColor::Red:
            return row == 0;
        case PlayerColor::Blue:
            return row == (m_board.getRowCount() - 1);
        case PlayerColor::Green:
            return col == 0;
        default:
            return false;
    }
}

bool Game::isInsufficientMaterialDraw() const
{
    for (const auto& player : m_players)
    {
        if (player->isEliminated())
        {
            continue;
        }

        for (Piece* piece : player->getPieces())
        {
            if (piece != nullptr && !piece->isCaptured() && piece->getType() != PieceType::King)
            {
                return false;
            }
        }
    }

    return true;
}

bool Game::wouldCaptureEnemyKing(const Move& move, PlayerColor movingColor) const
{
    const Square* toSquare = move.resolveTo(m_board);
    if (toSquare == nullptr || !toSquare->isOccupied())
    {
        return false;
    }

    Piece* targetPiece = toSquare->getPiece();
    return targetPiece != nullptr
        && targetPiece->getType() == PieceType::King
        && targetPiece->getOwner() != nullptr
        && targetPiece->getOwner()->getColor() != movingColor;
}

bool Game::applyMoveInternal(const Move& move, MoveStateSnapshot& snapshot)
{
    if (!move.isValid())
    {
        return false;
    }

    Square* fromSquare = move.resolveFrom(m_board);
    Square* toSquare = move.resolveTo(m_board);

    if (fromSquare == nullptr || toSquare == nullptr || !fromSquare->isOccupied())
    {
        return false;
    }

    Piece* movedPiece = fromSquare->getPiece();
    if (movedPiece == nullptr)
    {
        return false;
    }

    snapshot.movedPiece = movedPiece;
    snapshot.movedPieceOriginalType = movedPiece->getType();
    snapshot.movedPieceOriginalHasMoved = movedPiece->hasMoved();
    snapshot.fromSquare = fromSquare;
    snapshot.toSquare = toSquare;
    snapshot.enPassantTargetBefore = m_enPassantTargetSquareId;
    snapshot.enPassantVictimBefore = m_enPassantVictimSquareId;
    snapshot.enPassantRemainingBefore = m_enPassantRemainingResponses;

    if (move.getType() == MoveType::EnPassant)
    {
        Square* victimSquare = m_board.getSquare(m_enPassantVictimSquareId);
        if (victimSquare == nullptr || !victimSquare->isOccupied())
        {
            return false;
        }

        snapshot.capturedPiece = victimSquare->getPiece();
        snapshot.capturedPieceOriginalSquare = victimSquare;
        snapshot.capturedPieceOriginalCapturedState = snapshot.capturedPiece->isCaptured();

        victimSquare->removePiece();
        snapshot.capturedPiece->setSquare(nullptr);
        snapshot.capturedPiece->setCaptured(true);
    }
    else if (toSquare->isOccupied())
    {
        snapshot.capturedPiece = toSquare->getPiece();
        snapshot.capturedPieceOriginalSquare = toSquare;
        snapshot.capturedPieceOriginalCapturedState = snapshot.capturedPiece->isCaptured();

        toSquare->removePiece();
        snapshot.capturedPiece->setSquare(nullptr);
        snapshot.capturedPiece->setCaptured(true);
    }

    snapshot.fromSquare->removePiece();
    snapshot.toSquare->setPiece(movedPiece);
    movedPiece->setSquare(snapshot.toSquare);
    movedPiece->setHasMoved(true);
    movedPiece->setCaptured(false);

    if (move.getType() == MoveType::Castling)
    {
        const int row = m_board.getRow(snapshot.fromSquare->getId());
        const int fromCol = m_board.getCol(snapshot.fromSquare->getId());
        const int toCol = m_board.getCol(snapshot.toSquare->getId());
        const int direction = (toCol > fromCol) ? 1 : -1;

        int scanCol = toCol + direction;
        while (m_board.isValid(row, scanCol))
        {
            Square* square = m_board.getSquare(row, scanCol);
            if (square != nullptr && square->isOccupied())
            {
                Piece* rook = square->getPiece();
                if (rook != nullptr
                    && rook->getOwner() == movedPiece->getOwner()
                    && rook->getType() == PieceType::Rook)
                {
                    snapshot.rookPiece = rook;
                    snapshot.rookFromSquare = square;
                    snapshot.rookOriginalHasMoved = rook->hasMoved();
                    snapshot.rookToSquare = m_board.getSquare(row, fromCol + direction);

                    if (snapshot.rookToSquare == nullptr)
                    {
                        return false;
                    }

                    snapshot.rookFromSquare->removePiece();
                    snapshot.rookToSquare->setPiece(rook);
                    rook->setSquare(snapshot.rookToSquare);
                    rook->setHasMoved(true);
                }
                break;
            }

            scanCol += direction;
        }
    }

    if (move.getType() == MoveType::Promotion)
    {
        movedPiece->setType(move.getPromotionType());
    }

    updateEnPassantStateAfterMove(snapshot, move);
    return true;
}

void Game::undoMoveInternal(const MoveStateSnapshot& snapshot)
{
    if (snapshot.movedPiece == nullptr || snapshot.fromSquare == nullptr || snapshot.toSquare == nullptr)
    {
        return;
    }

    m_enPassantTargetSquareId = snapshot.enPassantTargetBefore;
    m_enPassantVictimSquareId = snapshot.enPassantVictimBefore;
    m_enPassantRemainingResponses = snapshot.enPassantRemainingBefore;

    snapshot.toSquare->removePiece();

    if (snapshot.rookPiece != nullptr && snapshot.rookFromSquare != nullptr && snapshot.rookToSquare != nullptr)
    {
        snapshot.rookToSquare->removePiece();
        snapshot.rookFromSquare->setPiece(snapshot.rookPiece);
        snapshot.rookPiece->setSquare(snapshot.rookFromSquare);
        snapshot.rookPiece->setHasMoved(snapshot.rookOriginalHasMoved);
    }

    snapshot.fromSquare->setPiece(snapshot.movedPiece);
    snapshot.movedPiece->setSquare(snapshot.fromSquare);
    snapshot.movedPiece->setType(snapshot.movedPieceOriginalType);
    snapshot.movedPiece->setHasMoved(snapshot.movedPieceOriginalHasMoved);
    snapshot.movedPiece->setCaptured(false);

    if (snapshot.capturedPiece != nullptr && snapshot.capturedPieceOriginalSquare != nullptr)
    {
        snapshot.capturedPieceOriginalSquare->setPiece(snapshot.capturedPiece);
        snapshot.capturedPiece->setSquare(snapshot.capturedPieceOriginalSquare);
        snapshot.capturedPiece->setCaptured(snapshot.capturedPieceOriginalCapturedState);
    }
}

void Game::clearEnPassantState()
{
    m_enPassantTargetSquareId = -1;
    m_enPassantVictimSquareId = -1;
    m_enPassantRemainingResponses = 0;
}

void Game::updateEnPassantStateAfterMove(const MoveStateSnapshot& snapshot, const Move& move)
{
    clearEnPassantState();

    if (snapshot.movedPiece == nullptr || snapshot.fromSquare == nullptr || snapshot.toSquare == nullptr)
    {
        return;
    }

    const bool movedPawn = snapshot.movedPieceOriginalType == PieceType::Pawn;
    const int fromRow = m_board.getRow(snapshot.fromSquare->getId());
    const int fromCol = m_board.getCol(snapshot.fromSquare->getId());
    const int toRow = m_board.getRow(snapshot.toSquare->getId());
    const int toCol = m_board.getCol(snapshot.toSquare->getId());

    if (move.getType() == MoveType::EnPassant)
    {
        return;
    }

    if (movedPawn)
    {
        const int rowDistance = std::abs(toRow - fromRow);
        const int colDistance = std::abs(toCol - fromCol);

        if ((rowDistance == 2 && colDistance == 0) || (rowDistance == 0 && colDistance == 2))
        {
            const int middleRow = (fromRow + toRow) / 2;
            const int middleCol = (fromCol + toCol) / 2;
            Square* middleSquare = m_board.getSquare(middleRow, middleCol);

            if (middleSquare != nullptr)
            {
                m_enPassantTargetSquareId = middleSquare->getId();
                m_enPassantVictimSquareId = snapshot.toSquare->getId();
                m_enPassantRemainingResponses = std::max(0, getActivePlayerCount() - 1);
                return;
            }
        }
    }

    if (snapshot.enPassantRemainingBefore > 1)
    {
        m_enPassantTargetSquareId = snapshot.enPassantTargetBefore;
        m_enPassantVictimSquareId = snapshot.enPassantVictimBefore;
        m_enPassantRemainingResponses = snapshot.enPassantRemainingBefore - 1;
    }
}

int Game::getActivePlayerCount() const
{
    int count = 0;

    for (const auto& player : m_players)
    {
        if (!player->isEliminated())
        {
            ++count;
        }
    }

    return count;
}

void Game::advanceToNextActivePlayer()
{
    if (m_players.empty() || getActivePlayerCount() == 0)
    {
        return;
    }

    for (std::size_t i = 0; i < m_players.size(); ++i)
    {
        m_currentPlayerIndex = (m_currentPlayerIndex + 1) % m_players.size();
        if (!m_players[m_currentPlayerIndex]->isEliminated())
        {
            return;
        }
    }
}

void Game::eliminatePlayer(Player& player)
{
    if (player.isEliminated())
    {
        return;
    }

    player.setEliminated(true);

    for (Piece* piece : player.getPieces())
    {
        if (piece == nullptr || piece->isCaptured())
        {
            continue;
        }

        if (piece->getSquare() != nullptr)
        {
            piece->getSquare()->removePiece();
        }

        piece->setSquare(nullptr);
        piece->setCaptured(true);
    }
}

void Game::eliminateCheckmatedPlayers()
{
    std::vector<Player*> playersToEliminate;

    for (const auto& player : m_players)
    {
        if (player->isEliminated())
        {
            continue;
        }

        if (isPlayerCheckmated(player->getColor()))
        {
            playersToEliminate.push_back(player.get());
        }
    }

    if (playersToEliminate.empty())
    {
        return;
    }

    std::ostringstream message;
    message << m_lastEvent;

    for (Player* player : playersToEliminate)
    {
        eliminatePlayer(*player);
        message << " | Elimination de " << player->getName() << " (roi mate).";
    }

    m_lastEvent = message.str();
}

Player* Game::findRemainingActivePlayer() const
{
    for (const auto& player : m_players)
    {
        if (!player->isEliminated())
        {
            return player.get();
        }
    }

    return nullptr;
}

void Game::refreshGameStateAfterMove()
{
    eliminateCheckmatedPlayers();

    if (getActivePlayerCount() == 0)
    {
        m_state = GameState::Draw;
        m_winner = PlayerColor::None;
        m_lastEvent += " | Nulle : aucun joueur actif restant.";
        return;
    }

    if (getActivePlayerCount() == 1)
    {
        Player* winner = findRemainingActivePlayer();
        m_state = GameState::WinByElimination;
        m_winner = (winner != nullptr) ? winner->getColor() : PlayerColor::None;
        if (winner != nullptr)
        {
            m_lastEvent += " | Victoire par elimination de " + winner->getName() + ".";
        }
        return;
    }

    if (isInsufficientMaterialDraw())
    {
        m_state = GameState::Draw;
        m_winner = PlayerColor::None;
        m_lastEvent += " | Nulle simplifiee : il ne reste que des rois.";
        return;
    }

    advanceToNextActivePlayer();

    Player* currentPlayer = getCurrentPlayer();
    if (currentPlayer == nullptr)
    {
        m_state = GameState::Draw;
        m_winner = PlayerColor::None;
        m_lastEvent += " | Nulle : aucun joueur courant.";
        return;
    }

    if (isPlayerStalemated(currentPlayer->getColor()))
    {
        m_state = GameState::Stalemate;
        m_winner = PlayerColor::None;
        m_lastEvent += " | Pat / nulle : " + currentPlayer->getName() + " n'a aucun coup legal.";
        return;
    }

    if (isPlayerInCheck(currentPlayer->getColor()))
    {
        m_state = GameState::Check;
        m_winner = PlayerColor::None;
        m_lastEvent += " | Alerte : " + currentPlayer->getName() + " est en echec.";
        return;
    }

    m_state = GameState::Ongoing;
    m_winner = PlayerColor::None;
}

bool Game::playMove(const Move& move)
{
    Player* currentPlayer = getCurrentPlayer();
    if (currentPlayer == nullptr || currentPlayer->isEliminated())
    {
        return false;
    }

    const std::vector<Move> legalMoves = getLegalMovesForPlayer(currentPlayer->getColor());
    const auto foundMove = std::find(legalMoves.begin(), legalMoves.end(), move);

    if (foundMove == legalMoves.end())
    {
        return false;
    }

    MoveStateSnapshot snapshot;
    if (!applyMoveInternal(*foundMove, snapshot))
    {
        return false;
    }

    std::ostringstream message;
    message << currentPlayer->getName() << " joue " << formatMoveForMessage(*foundMove);
    m_lastEvent = message.str();

    refreshGameStateAfterMove();
    return true;
}

GameState Game::getState() const
{
    return m_state;
}

PlayerColor Game::getWinner() const
{
    return m_winner;
}

const std::string& Game::getLastEvent() const
{
    return m_lastEvent;
}

void Game::printBoard(std::ostream& output) const
{
    output << "    ";
    for (int col = 0; col < m_board.getColCount(); ++col)
    {
        output << std::setw(4) << col;
    }
    output << '\n';

    for (int row = 0; row < m_board.getRowCount(); ++row)
    {
        output << std::setw(3) << row << ' ';

        for (int col = 0; col < m_board.getColCount(); ++col)
        {
            const Square* square = m_board.getSquare(row, col);
            std::string cell = "..";

            if (square != nullptr && square->isOccupied())
            {
                Piece* piece = square->getPiece();
                if (piece != nullptr && piece->getOwner() != nullptr)
                {
                    cell.clear();
                    cell.push_back(toPlayerLetter(piece->getOwner()->getColor()));
                    cell.push_back(toPieceLetter(piece->getType()));
                }
            }

            output << std::setw(4) << cell;
        }

        output << '\n';
    }
} 
