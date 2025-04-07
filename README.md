# Analyseurs Syntaxiques en C

Ce projet implémente deux types d'analyseurs syntaxiques en langage C :

## 1. Analyseur Syntaxique Descendant LL(1)

L'analyseur LL(1) est un analyseur descendant qui comprend les algorithmes suivants :

1. Élimination de la récursivité à gauche
2. Calcul des ensembles FIRST pour chaque non-terminal
3. Calcul des ensembles FOLLOW pour chaque non-terminal
4. Construction de la table d'analyse LL(1)
5. Exploitation de la table LL(1) pour analyser des chaînes d'entrée

## 2. Analyseur Syntaxique Ascendant SLR

L'analyseur SLR est un analyseur ascendant qui comprend les algorithmes suivants :

1. Construction de l'automate LR(0)
2. Calcul des ensembles FIRST pour chaque non-terminal
3. Calcul des ensembles FOLLOW pour chaque non-terminal
4. Construction de la table d'analyse SLR
5. Exploitation de la table SLR pour analyser des chaînes d'entrée

## Structure du projet

### Analyseur LL(1)
- `grammar.h` : Définition des structures de données et prototypes de fonctions
- `grammar.c` : Implémentation des algorithmes d'analyse syntaxique
- `main.c` : Programme principal avec des exemples d'utilisation

### Analyseur SLR
- `slr_parser.h` : Définition des structures de données et prototypes de fonctions
- `slr_parser_complete.c` et `slr_parser_part2.c` : Implémentation des algorithmes d'analyse syntaxique
- `slr_main.c` : Programme principal avec des exemples d'utilisation

- `README.md` : Documentation du projet

## Compilation et Exécution du Projet

Pour utiliser ce projet, vous devez d'abord installer un compilateur C sur votre système. Voici quelques options :

- **Windows** : MinGW-w64, Visual Studio Community, ou Code::Blocks avec MinGW
- **Linux** : GCC (généralement préinstallé)
- **macOS** : Xcode Command Line Tools (qui inclut GCC/Clang)

### Compilation et exécution de l'analyseur LL(1)

1. Ouvrez un terminal ou une invite de commande
2. Naviguez jusqu'au répertoire du projet
3. Compilez l'analyseur LL(1) avec la commande :

```bash
# Sur Linux/macOS
gcc -o ll1_parser main.c grammar.c

# Sur Windows avec MinGW
gcc -o ll1_parser.exe main.c grammar.c

# Sur Windows avec Visual Studio (Invite de commande pour VS)
cl /Fe:ll1_parser.exe main.c grammar.c
```

4. Exécutez l'analyseur :

```bash
# Sur Linux/macOS
./ll1_parser

# Sur Windows
ll1_parser.exe
```

### Compilation et exécution de l'analyseur SLR

1. Ouvrez un terminal ou une invite de commande
2. Naviguez jusqu'au répertoire du projet
3. Compilez l'analyseur SLR avec la commande :

```bash
# Sur Linux/macOS
gcc -o slr_parser slr_main.c slr_parser_complete.c slr_parser_part2.c

# Sur Windows avec MinGW
gcc -o slr_parser.exe slr_main.c slr_parser_complete.c slr_parser_part2.c

# Sur Windows avec Visual Studio (Invite de commande pour VS)
cl /Fe:slr_parser.exe slr_main.c slr_parser_complete.c slr_parser_part2.c
```

4. Exécutez l'analyseur :

```bash
# Sur Linux/macOS
./slr_parser

# Sur Windows
slr_parser.exe
```

## Fonctionnalités

### Analyseur LL(1)

#### 1. Élimination de la récursivité à gauche
L'algorithme transforme les règles avec récursivité à gauche (A → Aα | β) en règles équivalentes sans récursivité à gauche (A → βA', A' → αA' | ε).

#### 2. Calcul des ensembles FIRST
Pour chaque non-terminal A, l'algorithme calcule l'ensemble des terminaux qui peuvent apparaître au début d'une dérivation de A.

#### 3. Calcul des ensembles FOLLOW
Pour chaque non-terminal A, l'algorithme calcule l'ensemble des terminaux qui peuvent suivre A dans une dérivation.

#### 4. Construction de la table LL(1)
L'algorithme construit la table d'analyse LL(1) en utilisant les ensembles FIRST et FOLLOW.

#### 5. Exploitation de la table LL(1)
L'algorithme utilise la table d'analyse pour analyser une chaîne d'entrée et déterminer si elle appartient au langage défini par la grammaire.

### Analyseur SLR

#### 1. Construction de l'automate LR(0)
L'algorithme construit l'automate LR(0) à partir de la grammaire, en calculant les états et les transitions entre eux.

#### 2. Calcul des ensembles FIRST
Pour chaque non-terminal A, l'algorithme calcule l'ensemble des terminaux qui peuvent apparaître au début d'une dérivation de A.

#### 3. Calcul des ensembles FOLLOW
Pour chaque non-terminal A, l'algorithme calcule l'ensemble des terminaux qui peuvent suivre A dans une dérivation.

#### 4. Construction de la table SLR
L'algorithme construit la table d'analyse SLR en utilisant l'automate LR(0) et les ensembles FOLLOW.

#### 5. Exploitation de la table SLR
L'algorithme utilise la table d'analyse pour analyser une chaîne d'entrée et déterminer si elle appartient au langage défini par la grammaire.

## Exemples d'utilisation

### Analyseur LL(1)
Le programme principal (`main.c`) contient deux exemples :

1. Une grammaire d'expressions arithmétiques avec récursivité à gauche
2. Une grammaire simple pour tester les algorithmes

Pour chaque exemple, le programme affiche :
- La grammaire originale
- La grammaire après élimination de la récursivité à gauche (pour le premier exemple)
- Les ensembles FIRST et FOLLOW
- La table d'analyse LL(1)
- L'analyse d'une chaîne d'entrée

### Analyseur SLR
Le programme principal (`slr_main.c`) contient également deux exemples :

1. Une grammaire d'expressions arithmétiques
2. Une grammaire simple pour tester les algorithmes

Pour chaque exemple, le programme affiche :
- La grammaire
- L'automate LR(0)
- Les ensembles FIRST et FOLLOW
- La table d'analyse SLR
- L'analyse d'une chaîne d'entrée

## Représentation de la grammaire

- Les non-terminaux sont représentés par des lettres majuscules (A, B, C, ...)
- Les terminaux sont représentés par des lettres minuscules ou des symboles (a, b, +, *, ...)
- Le symbole epsilon (ε) est représenté par '$'
