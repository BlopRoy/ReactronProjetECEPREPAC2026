#ifndef GESTION_MATRICE_H
#define GESTION_MATRICE_H

#include <stdbool.h>
#include "type.h"

// remplit la grille au démarrage avec des symboles aléatoires
// fait attention à ne pas créer de triplet dès le départ
// recommence jusqu'à avoir une grille jouable
void init_matrix(GameContext *ctx);

// échange de deux cases adjacentes si cela permet un alignement
// si cela ne permet pas d'atteindre un triplet, l'échange est annulé et on retourne false
bool swap_cells(GameContext *ctx, int x1, int y1, int x2, int y2);

// fait tomber toutes les cellules vers le bas pour combler les trous
// retourne true si au moins une cellule a bougé
bool apply_gravity(GameContext *ctx);

// génère de nouveaux symboles en haut de chaque colonne vide après la gravité
void refill_top(GameContext *ctx);

// regarde si la grille est bloquée et qu'aucun triplet n'est possibe, si oui en génère une nouvelle
// le score est conservé
void regenerate_if_blocked(GameContext *ctx);

// teste toutes les permutations possibles pour voir s'il reste un coup jouable
bool has_valid_moves(GameContext *ctx);

// retourne true si au moins une case vide traîne encore dans la grille
bool has_empty_cell(GameContext *ctx);

#endif