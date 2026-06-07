#include "detection_alignements.h"

/**
 * @brief Détecte les alignements de cellules identiques et marque celles à supprimer.
 * @details Parcourt la matrice horizontalement puis verticalement (alignements >= 3).
 * Active un effet global ("bombe") pour tout type d'énergie aligné 5 fois ou plus.
 * @param ctx Pointeur vers le contexte général du jeu (matrice, états).
 * @return true si au moins un alignement a été trouvé, false sinon.
 */
bool detect_and_mark(GameContext *ctx) {
    // Indicateur de présence d'au moins un alignement supprimable dans ce cycle
    bool found = false;
    
    // Tableau temporaire local pour stocker les types de bombes à déclencher (combo >= 5)
    // Permet de mémoriser les types d'énergie qui doivent subir une disparition totale
    bool trigger_bomb_for_type[CELL_COUNT] = { false };

    // =========================================================================
    // 1. SCAN HORIZONTAL
    // =========================================================================
    for (int r = 0; r < MAP_ROWS; r++) {
        int match_count = 1; // Compteur de cellules consécutives identiques
        for (int c = 1; c <= MAP_COLS; c++) {
            // Si on est dans les limites et que la cellule courante est identique à la précédente (et non vide)
            if (c < MAP_COLS && ctx->matrix[r][c] == ctx->matrix[r][c-1] && ctx->matrix[r][c] != CELL_EMPTY) {
                match_count++;
            } else {
                // Si la série s'arrête, on vérifie si sa longueur est valide pour une absorption (au moins 3)
                if (match_count >= 3) {
                    found = true;
                    CellType type_aligne = ctx->matrix[r][c-1];
                    
                    // Marquage de toutes les cellules de cette suite horizontale dans le tableau dédié
                    for (int k = c - match_count; k < c; k++) {
                        ctx->marked[r][k] = 1;
                    }
                    // Règle spéciale : un alignement de 5 déclenche une réaction en chaîne sur ce type d'énergie
                    if (match_count >= 5) {
                        trigger_bomb_for_type[type_aligne] = true;
                    }
                }
                match_count = 1; // Réinitialisation du compteur pour le prochain alignement sur la ligne
            }
        }
    }

    // =========================================================================
    // 2. SCAN VERTICAL
    // =========================================================================
    for (int c = 0; c < MAP_COLS; c++) {
        int match_count = 1; // Compteur de cellules consécutives identiques
        for (int r = 1; r <= MAP_ROWS; r++) {
            // Si on est dans les limites et que la cellule courante est identique à celle du dessus (et non vide)
            if (r < MAP_ROWS && ctx->matrix[r][c] == ctx->matrix[r-1][c] && ctx->matrix[r][c] != CELL_EMPTY) {
                match_count++;
            } else {
                // Si la série s'arrête, on vérifie si sa longueur est valide (au moins 3)
                if (match_count >= 3) {
                    found = true;
                    CellType type_aligne = ctx->matrix[r-1][c];
                    
                    // Marquage de toutes les cellules de cette suite verticale
                    for (int k = r - match_count; k < r; k++) {
                        ctx->marked[k][c] = 1;
                    }
                    // Règle spéciale : un alignement vertical de 5 déclenche également l'effet global
                    if (match_count >= 5) {
                        trigger_bomb_for_type[type_aligne] = true;
                    }
                }
                match_count = 1; // Réinitialisation du compteur pour la prochaine suite sur la colonne
            }
        }
    }

    // =========================================================================
    // 3. APPLICATION DE L'EFFET "BOMBE GLOBALE"
    // =========================================================================
    // Si un alignement d'une taille >= 5 a été enregistré, on marque TOUTES les cellules
    // de la matrice qui possèdent ce type d'énergie, peu importe leur position.
    for (int t = 1; t < CELL_COUNT; t++) {
        if (trigger_bomb_for_type[t]) {
            for (int r = 0; r < MAP_ROWS; r++) {
                for (int c = 0; c < MAP_COLS; c++) {
                    if (ctx->matrix[r][c] == (CellType)t) {
                        ctx->marked[r][c] = 1; // Forçage du marquage pour destruction globale
                    }
                }
            }
        }
    }

    return found; // Renvoie vrai si des modifications ont été planifiées
}

/**
 * @brief Applique la suppression effective des cellules marquées et met à jour les scores.
 * @details Parcourt la matrice pour vider les cellules marquées (CELL_EMPTY) et
 * incrémente les compteurs d'énergie correspondants aux objectifs du niveau.
 * @param ctx Pointeur vers le contexte général du jeu.
 * @return int Le nombre total de cellules détruites lors de cette étape.
 */
int resolve_suppression(GameContext *ctx) {
    int total_destroyed = 0; // Compteur des absorptions effectuées dans ce cycle
    
    // Parcours de l'intégralité de la matrice du réacteur
    for (int r = 0; r < MAP_ROWS; r++) {
        for (int c = 0; c < MAP_COLS; c++) {
            // Si la cellule a été marquée lors de la phase de détection
            if (ctx->marked[r][c] == 1) {
                CellType current_type = ctx->matrix[r][c];
                
                // Incrémentation sécurisée des compteurs d'énergie
                // On vérifie que le type est valide pour éviter tout débordement d'indice
                if (current_type > CELL_EMPTY && current_type < CELL_COUNT) {
                    ctx->level.cur_energy[current_type]++;
                }
                
                ctx->matrix[r][c] = CELL_EMPTY; // Vidage de la cellule (absorption par le réacteur)
                ctx->marked[r][c] = 0;          // Réinitialisation du marqueur pour le prochain tour
                total_destroyed++;              // Mise à jour du bilan de destruction
            }
        }
    }
    return total_destroyed; // Retourne le nombre d'unités d'énergie absorbées
}