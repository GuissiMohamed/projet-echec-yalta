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

```text
include/
├── core/      → logique du jeu (Board, Piece, Game…)
├── mvc/       → interface et contrôleur (ConsoleView, GameController)
├── patterns/  → pattern Command
└── strategy/  → pattern Strategy

src/
├── core/      → implémentation du moteur
├── mvc/       → implémentation MVC
├── patterns/  → implémentation Command
├── strategy/  → implémentation Strategy
└── main.cpp   → point d’entrée