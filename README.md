# Yalta Chess - Projet Universitaire

## Description
Implémentation du jeu d’échecs Yalta à 3 joueurs.

## Technologies
- C++17
- CMake
- SFML (à venir)
- PlantUML

## Architecture
- MVC
- Strategy
- Command

## Avancement
- S01 : Initialisation
- S02 : UML et analyse


rm -rf build
cmake -S . -B build
cmake --build build
./build/projet-echec-yalta