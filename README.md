# projet-echec-yalta ♟️

Projet universitaire : implémentation d’un jeu d’échecs **Yalta à 3 joueurs** en C++.

---

## 📌 Objectif

Développer un moteur de jeu complet avec :

- Plateau de **96 cases**
- **3 joueurs**
- Règles avancées :
  - échec / mat
  - roque
  - promotion
  - prise en passant
  - élimination d’un joueur
- Architecture logicielle propre (**MVC**)
- Interface interactive
- IA (à venir)

---

## ⚙️ Technologies utilisées

- **C++17**
- **CMake**
- Programmation orientée objet


---
## 📁 Structure du projet

include/
├── core/      → logique du jeu (Board, Piece, Game…)
├── mvc/       → interface et contrôleur (ConsoleView, GameController)

src/
├── core/      → implémentation du moteur
├── mvc/       → implémentation MVC
└── main.cpp   → point d’entrée
---

🧠 Architecture

Le projet suit le pattern MVC :

Model : Game, Board, Square, Piece, Move, Player
View : ConsoleView
Controller : GameController
🚀 Compilation et exécution

Depuis la racine du projet :

rm -rf build
cmake -S . -B build
cmake --build build
./build/projet-echec-yalta
🎮 Fonctionnalités actuelles (S06)
Génération des coups pseudo-légaux
Règles avancées implémentées :
échec / mat
roque
promotion
prise en passant
élimination d’un joueur
Interface console interactive
Architecture MVC en place
⚠️ Limitations actuelles
Représentation du plateau simplifiée (grille 8x12)
Interface uniquement console (pas encore graphique)
IA non implémentée
📈 Roadmap
S07 : Design patterns (Command, Strategy)
S08 : Interface graphique (SFML)
S09 : IA (Min-Max / Alpha-Beta)
S10 : Finalisation, tests et documentation


