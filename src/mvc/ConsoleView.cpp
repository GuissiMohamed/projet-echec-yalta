#include "mvc/ConsoleView.hpp"

#include <iostream>
#include <limits>

void ConsoleView::showWelcome() const
{
    std::cout << "============================================================\n";
    std::cout << "projet-echec-yalta - S06 - Interface console MVC\n";
    std::cout << "============================================================\n";
    std::cout << "Commandes disponibles :\n";
    std::cout << "  - play : jouer un coup\n";
    std::cout << "  - quit : quitter la partie\n\n";
    std::cout << "La saisie des coups se fait via les identifiants numeriques des cases.\n";
    std::cout << "Cette interface est temporaire et prepare la future interface graphique.\n\n";
}

void ConsoleView::showBoard(const Game& game) const
{
    game.printBoard(std::cout);
    std::cout << '\n';
}

void ConsoleView::showStatus(const Game& game) const
{
    const Player* currentPlayer = game.getCurrentPlayer();

    std::cout << "Etat du moteur : " << toString(game.getState()) << '\n';

    if (currentPlayer != nullptr)
    {
        std::cout << "Joueur courant : "
                  << currentPlayer->getName()
                  << " ("
                  << toString(currentPlayer->getColor())
                  << ")\n";
    }

    std::cout << "Dernier evenement : " << game.getLastEvent() << "\n\n";
}

void ConsoleView::showMoves(const std::vector<Move>& moves) const
{
    std::cout << "Coups legaux disponibles (" << moves.size() << ") :\n";

    for (const Move& move : moves)
    {
        if (!move.isValid())
        {
            continue;
        }

        std::cout << "  - " << move.getFromId()
                  << " -> " << move.getToId()
                  << " ["
                  << toString(move.getType())
                  << "]";

        if (move.getType() == MoveType::Promotion)
        {
            std::cout << " => " << toString(move.getPromotionType());
        }

        std::cout << '\n';
    }

    std::cout << '\n';
}

void ConsoleView::showMessage(const std::string& message) const
{
    std::cout << message << '\n';
}

void ConsoleView::showEnd(const Game& game) const
{
    std::cout << "\n============================================================\n";
    std::cout << "Fin de partie / fin de session\n";
    std::cout << "============================================================\n";
    std::cout << "Etat final : " << toString(game.getState()) << '\n';

    if (game.getWinner() != PlayerColor::None)
    {
        std::cout << "Vainqueur : " << toString(game.getWinner()) << '\n';
    }

    std::cout << "Dernier evenement : " << game.getLastEvent() << '\n';
}

std::string ConsoleView::askCommand() const
{
    std::cout << "Entrez une commande (play / quit) : ";

    std::string command;
    std::cin >> command;
    return command;
}

int ConsoleView::askSquareId(const std::string& prompt) const
{
    while (true)
    {
        std::cout << prompt;

        int squareId = -1;
        if (std::cin >> squareId)
        {
            return squareId;
        }

        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Saisie invalide. Entrez un nombre entier.\n";
    }
}

int ConsoleView::askPromotionChoice() const
{
    while (true)
    {
        std::cout << "Choisissez la promotion :\n";
        std::cout << "  1 - Queen\n";
        std::cout << "  2 - Rook\n";
        std::cout << "  3 - Bishop\n";
        std::cout << "  4 - Knight\n";
        std::cout << "Votre choix : ";

        int choice = 0;
        if (std::cin >> choice && choice >= 1 && choice <= 4)
        {
            return choice;
        }

        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Choix invalide.\n";
    }
}