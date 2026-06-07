#include "gestion_matrice.h"
#include "detection_alignements.h"
#include <stdlib.h>

// Permet de vérifier si la cellule à ses coordonnées est dans un alignement de 3 énergies
static bool check_immediate_match(GameContext *ctx, int r, int c, CellType type) {
    if (c >= 2 && ctx->matrix[r][c-1] == type && ctx->matrix[r][c-2] == type) return true;
    if (r >= 2 && ctx->matrix[r-1][c] == type && ctx->matrix[r-2][c] == type) return true;
    return false;
}

void init_matrix(GameContext *ctx) {
    bool valid_grid = false;
    
    while (!valid_grid) {
        // Remplissage de la matrice sans alignement de 3 énergies 
        for (int r = 0; r < MAP_ROWS; r++) {
            for (int c = 0; c < MAP_COLS; c++) {
                CellType t;
                int attempts = 0;
                do {
                    // Sélection aléatoire d'une cellule de CELL_R (1) à CELL_V (5)
                    t = (CellType)(rand() % (CELL_COUNT - 1) + 1);
                    attempts++;
                } while (check_immediate_match(ctx, r, c, t) && attempts < 10);
                
                ctx->matrix[r][c] = t;
                ctx->marked[r][c] = 0;
            }
        }
        
        // assure qu'un coup peut etre jouer, sinon on reboucle
        if (has_valid_moves(ctx)) {
            valid_grid = true;
        }
    }
    
    // Réinitialisation du curseur
    ctx->cursor_x = 0;
    ctx->cursor_y = 0;
    ctx->selected_x = -1;
    ctx->selected_y = -1;
}

bool swap_cells(GameContext *ctx, int x1, int y1, int x2, int y2) {
    // On vérifie les limites de la grille de jeu 
    if (x1 < 0 || x1 >= MAP_COLS || y1 < 0 || y1 >= MAP_ROWS ||
        x2 < 0 || x2 >= MAP_COLS || y2 < 0 || y2 >= MAP_ROWS) {
        return false;
    }

    // Permutation temporaire
    CellType temp = ctx->matrix[y1][x1];
    ctx->matrix[y1][x1] = ctx->matrix[y2][x2];
    ctx->matrix[y2][x2] = temp;

    // Simulation de détection 
    if (detect_and_mark(ctx)) {
        // Le coup est bien valide.
        // On remet à 0 les marquages 
        for (int r = 0; r < MAP_ROWS; r++) {
            for (int c = 0; c < MAP_COLS; c++) {
                ctx->marked[r][c] = 0;
            }
        }
        return true; 
    }

    // Aucun alignement trouvé : annulation de la permutation
    temp = ctx->matrix[y1][x1];
    ctx->matrix[y1][x1] = ctx->matrix[y2][x2];
    ctx->matrix[y2][x2] = temp;
    
    return false;
}

bool apply_gravity(GameContext *ctx) {
    bool moved = false;
    
    // On parcourt la grille de bas en haut
    for (int c = 0; c < MAP_COLS; c++) {
        for (int r = MAP_ROWS - 2; r >= 0; r--) {
            if (ctx->matrix[r][c] != CELL_EMPTY && ctx->matrix[r+1][c] == CELL_EMPTY) {
                // Descente de la cellule au maximul possible vers le bas
                int current_r = r;
                while (current_r + 1 < MAP_ROWS && ctx->matrix[current_r+1][c] == CELL_EMPTY) {
                    ctx->matrix[current_r+1][c] = ctx->matrix[current_r][c];
                    ctx->matrix[current_r][c] = CELL_EMPTY;
                    current_r++;
                    moved = true;
                }
            }
        }
    }
    return moved;
}

void refill_top(GameContext *ctx) {
    // Si ligne du haut vide, génération d'une nouvelle cellule aléatoire
    for (int c = 0; c < MAP_COLS; c++) {
        if (ctx->matrix[0][c] == CELL_EMPTY) {
            ctx->matrix[0][c] = (CellType)(rand() % (CELL_COUNT - 1) + 1);
        }
    }
}

bool has_valid_moves(GameContext *ctx) {
    // Teste chaque permutation adjacente en horizontal et en vertical 
    for (int r = 0; r < MAP_ROWS; r++) {
        for (int c = 0; c < MAP_COLS; c++) {
            // Test à l'horizontal
            if (c + 1 < MAP_COLS) {
                CellType t = ctx->matrix[r][c];
                ctx->matrix[r][c] = ctx->matrix[r][c+1];
                ctx->matrix[r][c+1] = t;
                
                bool match = detect_and_mark(ctx);
                
                // Remise en état
                t = ctx->matrix[r][c];
                ctx->matrix[r][c] = ctx->matrix[r][c+1];
                ctx->matrix[r][c+1] = t;
                
                if (match) {
                    // Remise à zero des marques résiduelles de simulation
                    for (int i = 0; i < MAP_ROWS; i++)
                        for (int j = 0; j < MAP_COLS; j++) ctx->marked[i][j] = 0;
                    return true;
                }
            }
            // Test à la verticale
            if (r + 1 < MAP_ROWS) {
                CellType t = ctx->matrix[r][c];
                ctx->matrix[r][c] = ctx->matrix[r+1][c];
                ctx->matrix[r+1][c] = t;
                
                bool match = detect_and_mark(ctx);
                
                t = ctx->matrix[r][c];
                ctx->matrix[r][c] = ctx->matrix[r+1][c];
                ctx->matrix[r+1][c] = t;
                
                if (match) {
                    for (int i = 0; i < MAP_ROWS; i++)
                        for (int j = 0; j < MAP_COLS; j++) ctx->marked[i][j] = 0;
                    return true;
                }
            }
        }
    }
    return false;
}

void regenerate_if_blocked(GameContext *ctx) {
    if (!has_valid_moves(ctx)) {
        // On conserve le score et régénere la matrice
        init_matrix(ctx);
    }
}

bool has_empty_cell(GameContext * ctx) {
    for (int r = 0; r  < MAP_ROWS; r++) {
        for (int c = 0; c < MAP_COLS; c++) {
            CellType t = ctx->matrix[r][c];
            if(t == CELL_EMPTY) return true; 
        }
    }
    return false;
}