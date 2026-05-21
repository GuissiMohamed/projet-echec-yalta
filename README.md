# ♟️ Projet Échecs Yalta

## 📌 Description

Ce projet implémente un moteur de jeu d’échecs à 3 joueurs inspiré du jeu Yalta.

Il repose sur une architecture modulaire en C++ avec :
- un moteur de règles complet
- une intelligence artificielle basée sur alpha-beta
- une interface graphique avec SFML
- des tests automatisés

---

## 🧱 Architecture

Le projet est structuré en plusieurs modules :

- `core/` : logique du jeu (Game, Board, Piece, Move…)
- `mvc/` : interface graphique et contrôleur
- `ai/` : intelligence artificielle (AlphaBeta)
- `strategy/` : stratégies de décision
- `patterns/` : implémentation des design patterns
- `tests/` : tests unitaires

---

## 🎯 Fonctionnalités

✔ Jeu à 3 joueurs (Rouge, Bleu, Vert)  
✔ Génération des coups légaux  
✔ Gestion des règles :
- échec / échec et mat  
- pat  
- promotion  
- roque  
- prise en passant  

✔ Intelligence artificielle :
- algorithme Alpha-Beta
- profondeur configurable

✔ Interface graphique (SFML)

✔ Tests automatisés (CTest)

---

## 🧠 Intelligence Artificielle

L’IA repose sur l’algorithme **Alpha-Beta** :

- exploration des coups possibles
- simulation des états du jeu
- évaluation des positions
- optimisation avec élagage alpha-beta

---

## 🧩 Design Patterns utilisés

- **Strategy** → choix du type de joueur (humain / IA)
- **Command** → encapsulation des coups
- **MVC** → séparation logique / affichage / contrôle

---

## 🖥️ Compilation

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j

▶️ Exécution
./build/projet-echec-yalta
🧪 Tests
ctest --test-dir build --output-on-failure
📚 Documentation
Documentation générée avec Doxygen :

doxygen Doxyfile

Puis ouvrir :

html/index.html

🚀 Perspectives
amélioration du rendu graphique
optimisation de l’IA
adaptation complète au plateau réel Yalta