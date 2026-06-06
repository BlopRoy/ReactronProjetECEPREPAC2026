#include "raylib.h"
#include "type.h"
#include "gestion_matrice.h"
#include "detection_alignements.h"
#include "gestion_niveaux.h"
#include "rendering.h"
#include "sauvegarde.h"
#include <stdlib.h>
#include <time.h>

// Petite fonction utilitaire interne pour éviter d'importer string.h uniquement pour strcpy
void src_cpy(char *dest, const char *src, size_t n) {
    for (size_t i = 0; i < n && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    dest[n - 1] = '\0';
}


int main(void) {
    // Initialisation du générateur pseudo-aléatoire
    srand((unsigned int)time(NULL));

    // 1. Initialisations de Raylib
    const int screenWidth = 1100;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Reactron - Quantum Reactor Stabilizer");
    SetTargetFPS(60);

    if (!IsWindowReady()) {
    TraceLog(LOG_ERROR, "Impossible d'initialiser le moteur graphique Raylib. Fermeture du programme.");
    return -1; 
    }

    // 2. Initialisation du Contexte de Jeu Unique
    GameContext game = {0};
    game.state = STATE_MENU;
    
    // Tentative de chargement d'une session précédente
    if (!load_progress(&game)) {
        game.surcharge = 0; 
        game.current_level_idx = 1;
        game.operator_name[0] = '\0';
    }

    load_level_config(&game, game.current_level_idx);
    init_matrix(&game); 

    // Variable interne pour rythmer la cascade (visuel fluide frame-by-frame)
    float cascade_timer = 0.0f;
    const float CASCADE_DELAY = 0.15f; // Délai d'animation en secondes

    // 3. Boucle de jeu principale
    while (!WindowShouldClose()) {
        
        // --- MISE À JOUR DE LA LOGIQUE (Update) ---
        float dt = GetFrameTime();

        switch (game.state) {
            case STATE_MENU:
                if (IsKeyPressed(KEY_ENTER)) {
                    // Force un nom par défaut si resté vide
                    if (game.operator_name[0] == '\0') {
                        src_cpy(game.operator_name, "OP-77", 6);
                    }
                    save_progress(&game);
                    game.state = STATE_GAMEPLAY;
                }
                break;

            case STATE_GAMEPLAY:
                game.level.time_elapsed += dt;
                check_game_conditions(&game); 

                // Entrées Utilisateur : Déplacement dans la grille quantique
                if (IsKeyPressed(KEY_UP) && game.cursor_y > 0) game.cursor_y--; 
                if (IsKeyPressed(KEY_DOWN) && game.cursor_y < MAP_ROWS - 1) game.cursor_y++; 
                if (IsKeyPressed(KEY_LEFT) && game.cursor_x > 0) game.cursor_x--;
                if (IsKeyPressed(KEY_RIGHT) && game.cursor_x < MAP_COLS - 1) game.cursor_x++;

                // Gestion de la Sélection et Permutation
                if (IsKeyPressed(KEY_SPACE)) {
                    if (game.selected_x == -1) {
                        // Premier verrouillage de cellule
                        game.selected_x = game.cursor_x;
                        game.selected_y = game.cursor_y;
                    } else {
                        // Calcul de la distance de Manhattan (doit être adjacente stricte)
                        int dist = abs(game.selected_x - game.cursor_x) + abs(game.selected_y - game.cursor_y);
                        if (dist == 1) {
                            if (swap_cells(&game, game.selected_x, game.selected_y, game.cursor_x, game.cursor_y)) {
                                game.level.cur_moves++; // Incrémente le compteur de coups légaux
                                game.state = STATE_CASCADE; // Enclenchement de la réaction en chaîne
                            }
                        }
                        game.selected_x = -1; // Reset sélecteur dans tous les cas
                    }
                }
                break;

            case STATE_CASCADE:
                // Temporisation pour donner un effet visuel d'effondrement par étapes
                cascade_timer += dt;
                if (cascade_timer >= CASCADE_DELAY) {
                    cascade_timer = 0.0f;

                    if (detect_and_mark(&game)) {
                        resolve_suppression(&game);
                        while(has_empty_cell(&game)) {
                            apply_gravity(&game);
                            refill_top(&game);
                        }
                    } else {
                        // Fin de la réaction en chaîne, le coeur s'est stabilisé
                        regenerate_if_blocked(&game); 
                        check_game_conditions(&game); 
                        if (game.state == STATE_CASCADE) {
                            game.state = STATE_GAMEPLAY; 
                        }
                    }
                }
                break;

            case STATE_VICTORY:
                if (IsKeyPressed(KEY_N)) {
                    game.current_level_idx++; 
                    save_progress(&game); 
                    load_level_config(&game, game.current_level_idx);
                    init_matrix(&game);
                    game.state = STATE_GAMEPLAY;
                }
                break;

            case STATE_GAMEOVER:
                if (IsKeyPressed(KEY_R)) { 
                    handle_level_fail(&game); 
                    save_progress(&game); 
                    if (game.surcharge >= MAX_SURCHARGE) {
                        game.state = STATE_CLEAR; 
                    } else {
                        // Relance le niveau en conservant la dégradation de surcharge
                        load_level_config(&game, game.current_level_idx);
                        init_matrix(&game);
                        game.state = STATE_GAMEPLAY;
                    }
                }
                break;
                
            case STATE_CLEAR:
                if (IsKeyPressed(KEY_ENTER)) { 
                    // Wipe complet de la mémoire flash du réacteur
                    game.surcharge = 0;
                    game.current_level_idx = 1;
                    load_level_config(&game, 1);
                    init_matrix(&game);
                    save_progress(&game);
                    game.state = STATE_GAMEPLAY;
                }
                break;
        }

        // --- DESSIN DE L'INTERFACE (Draw) --- 
        BeginDrawing();
            ClearBackground(DARKGRAY); 

            switch (game.state) {
                case STATE_MENU:
                    DrawMenuScreen(&game);
                    break;
                case STATE_GAMEPLAY:
                case STATE_CASCADE:
                    DrawGridi(&game);      
                    DrawInterface(&game); 
                    break;
                case STATE_VICTORY:
                    DrawGridi(&game); // Dessin en arrière-plan pour effet visuel sympa
                    DrawInterface(&game);
                    DrawVictoryScreen(&game); 
                    break;
                case STATE_GAMEOVER:
                    DrawGridi(&game);
                    DrawInterface(&game);
                    DrawGameOverScreen(&game); 
                    break;
                case STATE_CLEAR:
                    DrawDefinitiveInstabilityScreen(&game); 
                    break;
            }
        EndDrawing();
    }

    // 4. Nettoyage et fermeture du contexte GPU
    CloseWindow();
    return 0;
}