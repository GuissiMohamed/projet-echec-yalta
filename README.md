# projet-echec-yalta

Projet universitaire de jeu d’échecs **Yalta à 3 joueurs** en **C++17** avec :
- moteur de jeu à **96 cases**
- architecture **MVC**
- design patterns **MVC / Command / Strategy**
- interface graphique **SFML**
- IA **alpha-beta multi-threadée**
- tests automatiques **CTest**

> Note honnête : le projet est jouable, démontrable et structuré, mais la **topologie exacte complète du plateau Yalta réel** reste encore **partiellement transitoire** dans la logique profonde du moteur. L’architecture a toutefois été préparée pour cette évolution et le rendu visuel du plateau a été rapproché du plateau réel.

---

## 1. Fonctionnalités principales

### Moteur
- partie à **3 joueurs** : Rouge / Bleu / Vert
- **96 cases**
- gestion des coups légaux
- détection de l’échec
- élimination d’un joueur maté
- détection de la nulle simplifiée et du pat
- mouvements spéciaux :
  - roque
  - promotion
  - prise en passant

### Interface
- interface graphique **SFML**
- sélection des pièces et affichage des coups possibles
- panneau d’état de partie
- bannière de fin de partie
- rendu visuel du plateau proche du Yalta réel

### IA
- IA **alpha-beta**
- **multi-threadée** au niveau racine
- profondeur configurable
- nombre de threads configurable
- simulation possible de **1, 2 ou 3 joueurs IA**

### Qualité logicielle
- architecture modulaire
- UML
- tests de non-régression via **CTest**

---

## 2. Structure du projet

Le projet est organisé autour de plusieurs sous-ensembles :

- `core/`
  - `Board`
  - `Game`
  - `Move`
  - `Piece`
  - `Player`
  - `Square`
  - topologie du plateau
- `mvc/`
  - `SFMLView`
  - `SFMLGameController`
  - `ConsoleView`
  - `GameController`
- `patterns/`
  - `ICommand`
  - `MoveCommand`
- `strategy/`
  - stratégies de jeu / contrôle
- `ai/`
  - `AlphaBetaAI`
- `tests/`
  - tests moteur

---

## 3. Prérequis

### macOS
Outils utilisés :
- `git`
- `cmake`
- `homebrew`
- `SFML`

Installation typique de SFML avec Homebrew :

```bash
brew install sfml
```

---

## 4. Compilation

Depuis la racine du projet :

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
```

---

## 5. Lancement

### Mode standard
Rouge humain, Bleu IA, Vert IA :

```bash
./build/projet-echec-yalta --red human --blue ai --green ai --depth 3 --threads 4
```

### Trois IA
```bash
./build/projet-echec-yalta --red ai --blue ai --green ai --depth 2 --threads 6
```

### Trois humains
```bash
./build/projet-echec-yalta --red human --blue human --green human
```

### Aide
```bash
./build/projet-echec-yalta --help
```

---

## 6. Paramètres disponibles

- `--red human|ai`
- `--blue human|ai`
- `--green human|ai`
- `--depth N`
- `--threads N`

Exemple :

```bash
./build/projet-echec-yalta --red ai --blue human --green ai --depth 3 --threads 4
```

---

## 7. Tests

Exécution des tests :

```bash
ctest --test-dir build --output-on-failure
```

Les tests couvrent notamment :
- le plateau à 96 cases
- l’ajout de pièces
- la copie de partie
- les mouvements initiaux des pions
- la promotion
- la détection d’échec
- la rotation des tours

---

## 8. Choix de conception

### MVC
Le projet sépare :
- le **modèle** : moteur de jeu
- la **vue** : affichage console et SFML
- le **contrôleur** : gestion des interactions et de la boucle de partie

### Command
Les coups sont encapsulés via `MoveCommand`.

### Strategy
Les différents comportements de joueurs / contrôles ont été organisés selon une logique de stratégie.

### IA
L’IA utilise alpha-beta avec parallélisation au niveau racine.  
Pour le jeu à 3 joueurs, l’approche retenue est de type **paranoïde** :
- l’IA maximise pour sa couleur
- les deux autres joueurs sont traités comme adversaires agrégés

---

## 9. État de conformité au sujet

### Réalisé
- jeu à **3 joueurs**
- plateau à **96 cases**
- **16 pièces par joueur**
- modélisation UML
- **3 design patterns**
- **MVC obligatoire**
- interface graphique interactive
- IA **alpha-beta**
- IA **multi-threadée**
- simulation de **1 / 2 / 3 IA**
- dépôt Git
- tests moteur

### Réserve principale
Le projet n’implémente pas encore de manière totalement garantie la **topologie exacte complète du vrai plateau Yalta réel** dans toute la logique du moteur.  
C’est le principal point à annoncer honnêtement dans le rapport et à la soutenance.

---

## 10. Limites connues

- topologie exacte Yalta encore partiellement transitoire
- couverture de tests encore améliorable
- fonction d’évaluation de l’IA perfectible
- interface finale encore améliorable si l’on veut une fidélité visuelle maximale au plateau officiel

---

## 11. Livrables associés

Le rendu final comprend :
- le code source
- les UML
- le rapport final
- ce README
- les captures d’écran à ajouter au rapport
- les preuves de compilation / test

---

## 12. Formulation honnête recommandée

Une formulation honnête et défendable pour le rapport / la soutenance est la suivante :

> Le projet respecte l’architecture demandée, les patterns, l’interface interactive, l’IA alpha-beta multi-threadée et la gestion d’une partie à trois joueurs. En revanche, la topologie exacte complète du plateau Yalta réel reste encore partiellement transitoire dans le moteur, même si elle a été préparée architecturalement et améliorée visuellement.




./build/projet-echec-yalta --red human --blue ai --green ai --depth 3 --threads 4
./build/projet-echec-yalta --red ai --blue ai --green ai --depth 2 --threads 6
./build/projet-echec-yalta --red human --blue human --green human