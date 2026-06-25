#include "rendering.h"
#include <stdio.h>
#include <string.h>

#define CELL_SIZE 32
#define GRID_OFFSET_X 40
#define GRID_OFFSET_Y 120

Color GetCellColor(CellType type) {
    switch (type) {
        case CELL_R: return RED;       // Thermique
        case CELL_B: return BLUE;      // Cryogénique
        case CELL_G: return LIME;      // Plasma
        case CELL_Y: return GOLD;      // Photonique
        case CELL_V: return PURPLE;    // Gravitationnelle
        default:     return BLACK;     // CELL_EMPTY
    }
}

const char* GetCellName(CellType type) {
    switch (type) {
        case CELL_R: return "Thermique (R)";
        case CELL_B: return "Cryogenique (B)";
        case CELL_G: return "Plasma (G)";
        case CELL_Y: return "Photonique (Y)";
        case CELL_V: return "Gravitation (V)";
        default:     return "Vide";
    }
}

void DrawGridi(const GameContext *ctx) {
    // Dessin du conteneur extérieur du réacteur
    DrawRectangleLines(GRID_OFFSET_X - 4, GRID_OFFSET_Y - 4, (MAP_COLS * CELL_SIZE) + 8, (MAP_ROWS * CELL_SIZE) + 8, WHITE);

    for (int r = 0; r < MAP_ROWS; r++) {
        for (int c = 0; c < MAP_COLS; c++) {
            int posX = GRID_OFFSET_X + (c * CELL_SIZE);
            int posY = GRID_OFFSET_Y + (r * CELL_SIZE);

            CellType type = ctx->matrix[r][c];

            if (type != CELL_EMPTY) {
                // Rendu des noyaux d'énergie
                DrawRectangle(posX + 2, posY + 2, CELL_SIZE - 4, CELL_SIZE - 4, GetCellColor(type));
                DrawRectangleLines(posX + 2, posY + 2, CELL_SIZE - 4, CELL_SIZE - 4, Fade(WHITE, 0.3f));
            } else {
                // Case vide du réacteur
                DrawRectangle(posX + 2, posY + 2, CELL_SIZE - 4, CELL_SIZE - 4, Fade(GRAY, 0.1f));
            }

            // Affichage des feedbacks d'alignements imminents (en cascade)
            if (ctx->marked[r][c] == 1) {
                DrawRectangle(posX + 2, posY + 2, CELL_SIZE - 4, CELL_SIZE - 4, Fade(WHITE, 0.6f));
            }
        }
    }

    // Rendu graphique du curseur utilisateur
    int cursor_posX = GRID_OFFSET_X + (ctx->cursor_x * CELL_SIZE);
    int cursor_posY = GRID_OFFSET_Y + (ctx->cursor_y * CELL_SIZE);
    DrawRectangleLinesEx((Rectangle){ (float)cursor_posX, (float)cursor_posY, CELL_SIZE, CELL_SIZE }, 2.5f, SKYBLUE);

    // Rendu de la cellule sélectionnée pour couplage/permutation
    if (ctx->selected_x != -1 && ctx->selected_y != -1) {
        int sel_posX = GRID_OFFSET_X + (ctx->selected_x * CELL_SIZE);
        int sel_posY = GRID_OFFSET_Y + (ctx->selected_y * CELL_SIZE);
        DrawRectangleLinesEx((Rectangle){ (float)sel_posX, (float)sel_posY, CELL_SIZE, CELL_SIZE }, 2.5f, ORANGE);
    }
}

void DrawInterface(const GameContext *ctx) {
    int sidebar_X = GRID_OFFSET_X + (MAP_COLS * CELL_SIZE) + 40;

    // Titre et informations de l'opérateur
    DrawText("REACTRON CORE MONITOR", sidebar_X, 40, 22, GREEN);
    DrawText(TextFormat("Operateur: %s", ctx->operator_name[0] ? ctx->operator_name : "Inconnu"), sidebar_X, 75, 16, LIGHTGRAY);
    DrawText(TextFormat("Niveau : %d", ctx->current_level_idx), sidebar_X, 100, 16, WHITE);

    // Section 1: Contraintes énergétiques (Coups / Temps)
    DrawText(TextFormat("Coups restants : %d / %d", ctx->level.cur_moves, ctx->level.max_moves), sidebar_X, 140, 16, 
             (ctx->level.cur_moves >= ctx->level.max_moves - 5) ? RED : WHITE);

    float time_left = ctx->level.time_limit - ctx->level.time_elapsed;
    if (time_left < 0) time_left = 0;
    DrawText(TextFormat("Stabilite temps : %.1fs / %.1fs", time_left, ctx->level.time_limit), sidebar_X, 170, 16, 
             (time_left < 30.0f) ? RED : WHITE);
    
    // Jauge visuelle de temps
    float pct_time = time_left / ctx->level.time_limit;
    DrawRectangle(sidebar_X, 195, 240, 12, DARKGRAY);
    DrawRectangle(sidebar_X, 195, (int)(240 * pct_time), 12, (pct_time < 0.25f) ? RED : GOLD);

    // Section 2: Objectifs d'Énergie Quantique
    DrawText("OBJECTIFS ABSORPTION :", sidebar_X, 235, 16, BLUE);
    int current_ui_y = 265;
    for (int i = 1; i < CELL_COUNT; i++) {
        if (ctx->level.req_energy[i] > 0) {
            Color c = GetCellColor((CellType)i);
            DrawCircle(sidebar_X + 10, current_ui_y + 8, 7, c);
            
            DrawText(TextFormat("%s : %d/%d", GetCellName((CellType)i), ctx->level.cur_energy[i], ctx->level.req_energy[i]), 
                     sidebar_X + 28, current_ui_y, 14, 
                     (ctx->level.cur_energy[i] >= ctx->level.req_energy[i]) ? LIME : LIGHTGRAY);
            current_ui_y += 28;
        }
    }

    // Section 3: Sécurité Anti-Surcharge du Réacteur
    DrawText("BARRES DE SECURITE CORRUPTIONS :", sidebar_X, 460, 14, MAROON);
    for (int s = 0; s < MAX_SURCHARGE; s++) {
        Color bar_color = (s < ctx->surcharge) ? RED : DARKGRAY;
        DrawRectangle(sidebar_X + (s * 48), 490, 40, 18, bar_color);
        DrawRectangleLines(sidebar_X + (s * 48), 490, 40, 18, WHITE);
    }
    if (ctx->surcharge > 0) {
        DrawText(TextFormat("ATTENTION : Surcharge critique %d/%d !", ctx->surcharge, MAX_SURCHARGE), sidebar_X, 520, 13, RED);
    }
}

void DrawMenuScreen(GameContext *ctx) {
    DrawText("REACTRON", 250, 180, 50, GREEN);
    DrawText("QUANTUM REACTOR STABILIZER", 250, 240, 20, LIGHTGRAY);
    
    // Boîte de saisie basique pour le nom de l'opérateur via Raylib
    DrawText("Entrez le badge de l'operateur :", 250, 340, 16, WHITE);
    
    int key = GetCharPressed();
    size_t len = strlen(ctx->operator_name);
    while (key > 0) {
        if ((key >= 32) && (key <= 125) && (len < 15)) {
            ctx->operator_name[len] = (char)key;
            ctx->operator_name[len+1] = '\0';
        }
        key = GetCharPressed();
    }
    if (IsKeyPressed(KEY_BACKSPACE) && len > 0) {
        ctx->operator_name[len - 1] = '\0';
    }

    DrawRectangle(250, 370, 300, 35, BLACK);
    DrawRectangleLines(250, 370, 300, 35, GREEN);
    if (len > 0) {
        DrawText(ctx->operator_name, 260, 378, 18, LIME);
    } else {
        DrawText("LOG-ID...", 260, 378, 18, DARKGRAY);
    }

    DrawText("Appuyez sur [ ENTRÉE ] pour lancer le systeme core", 250, 460, 15, GOLD);
}

void DrawVictoryScreen(const GameContext *ctx) {
    DrawRectangle(0, 0, 1100, 600, Fade(BLACK, 0.8f));
    DrawText("CONCURRENCE STABILISÉE !", 220, 240, 38, LIME);
    DrawText(TextFormat("Le niveau %d est desormais sous controle.", ctx->current_level_idx), 220, 310, 18, WHITE);
    DrawText("Appuyez sur [ N ] pour initialiser le niveau suivant", 220, 400, 16, GOLD);
}

void DrawGameOverScreen(const GameContext *ctx) {
    DrawRectangle(0, 0, 1100, 600, Fade(BLACK, 0.85f));
    DrawText("RUPTURE DU CONFINEMENT CORE", 180, 240, 36, RED);
    DrawText("Le delai ou la limite de mouvements a expire.", 180, 310, 18, LIGHTGRAY);
    DrawText("Appuyez sur [ R ] pour purger le plasma et recommencer (+1 Surcharge)", 180, 400, 15, GOLD);
}

void DrawDefinitiveInstabilityScreen(const GameContext *ctx) {
    DrawRectangle(0, 0, 1100, 600, MAROON);
    DrawText("EVACUATION D'URGENCE SYSTEME", 150, 220, 40, WHITE);
    DrawText("CRITICAL ERROR: FUSION DU COEUR DU REACTEUR", 150, 290, 20, BLACK);
    DrawText("Les 5 barres de surcharge ont fondu sous la pression.", 150, 350, 16, LIGHTGRAY);
    DrawText("Appuyez sur [ ENTRÉE ] pour reinitialiser completement la grille", 150, 480, 16, GOLD);
}