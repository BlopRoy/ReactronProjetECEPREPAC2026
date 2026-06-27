#ifndef DETECTION_ALIGNEMENTS_H
#define DETECTION_ALIGNEMENTS_H

#include <stdbool.h>
#include "type.h"

//Ce module gère la détection et la suppression des alignements de cellules.

//Parcourt la matrice horizontalement puis verticalement pour trouver des alignements de 3 cellules identiques ou plus.
//Les cellules concernées sont marquées dans ctx->marked.
//Un alignement de 5 déclenche un effet "bombe" qui supprime tous les exemplaires de ce type.
//Retourne true si au moins un alignement a été trouvé.
bool detect_and_mark(GameContext *ctx);

//Supprime toutes les cellules marquées, incrémente les compteurs d'énergie du niveau en cours,
//et réinitialise les marqueurs pour le prochain cycle.
//Retourne le nombre total de cellules détruites.
int resolve_suppression(GameContext *ctx);

#endif