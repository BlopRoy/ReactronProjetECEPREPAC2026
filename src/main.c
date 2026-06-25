#include "raylib.h"
#include "type.h"
#include "gestion_matrice.h"
#include "detection_alignements.h"
#include "gestion_niveaux.h"
#include "rendering.h"
#include "sauvegarde.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <teste.h>
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

int main(void) {
    // Initialisation du générateur pseudo-aléatoire
    srand((unsigned int)time(NULL));

    // 1. Initialisations de Raylib
    const int screenWidth = 1100;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Reactron - Quantum Reactor Stabilizer");
    Texture2D background = LoadTexture("assets/cyberpunk_street_background.png");
    Texture2D midground = LoadTexture("assets/cyberpunk_street_midground.png");
    Texture2D foreground = LoadTexture("assets/cyberpunk_street_foreground.png");
    InitAudioDevice();              // Initialize audio device

    Music music = LoadMusicStream("assets/bgm_old.mp3");
    PlayMusicStream(music);
    bool pause = false;             // Music playing paused

    SetTargetFPS(60);

    if (!IsWindowReady()) {
        TraceLog(LOG_ERROR, "Impossible d'initialiser le moteur graphique Raylib. Fermeture du programme.");
        return -1; 
    }

    // 2. Initialisation du Contexte de Jeu Unique
    ScrollingBackground backg;
    GameContext game = {0};
    game.state = STATE_MENU;
    game.surcharge = 0; 
    game.current_level_idx = 1;
    game.operator_name[0] = '\0';
    backg.scrollingFore = backg.scrollingMid = backg.scrollingBack = 0.0f;

    init_save(&game);

    load_level_config(&game, game.current_level_idx);
    init_matrix(&game); 

    teste(&game);

    // Variable interne pour rythmer la cascade (visuel fluide frame-by-frame)
    float cascade_timer = 0.0f;
    const float CASCADE_DELAY = 0.15f; // Délai d'animation en secondes

    // 3. Boucle de jeu principale
    while (!WindowShouldClose()) {
        UpdateMusicStream(music);
        
        // --- MISE À JOUR DE LA LOGIQUE (Update) ---
        float dt = GetFrameTime();

        switch (game.state) {
            case STATE_MENU:
                if (IsKeyPressed(KEY_ENTER)) {

                    if (game.operator_name[0] == '\0') strcpy(game.operator_name, "OP-77");

                    if (!load_progress(&game)) {
                        game.surcharge = 0;
                        game.current_level_idx = 1;
                        init_save(&game);
                    }   

                    load_level_config(&game, game.current_level_idx);
                    init_matrix(&game);

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
                    teste(&game);
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
                    //teste(&game);
                }
                break;

            case STATE_VICTORY:
                if (IsKeyPressed(KEY_N)) {
                    game.current_level_idx++; 
                    save_progress(&game); 
                    load_level_config(&game, game.current_level_idx);
                    init_matrix(&game);
                    game.state = STATE_GAMEPLAY;
                    teste(&game);
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
                    teste(&game);
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
                    teste(&game);
                }
                break;
        }

        // --- DESSIN DE L'INTERFACE (Draw) --- 
        BeginDrawing();
        DrawMovingBackground(&backg,background,midground,foreground);
        // Multiplier la taille de l'icône par 2 (elle passera de 16x16 à 32x32)
        GuiSetIconScale(2);

        if (game.state != STATE_MENU) {
            if (GuiButton((Rectangle){75, 20, 40, 40}, GuiIconText(ICON_HOUSE, ""))) {
        
                // Si on était en train de jouer, on sécurise les données avant de quitter
                if (game.state == STATE_GAMEPLAY || game.state == STATE_CASCADE) {
                    save_progress(&game); 
                }
        
                game.state = STATE_MENU;
            }
        }

        // --- Dans votre bloc de dessin (BeginDrawing) ---

        // 1. Définir la couleur selon l'état de votre variable 'pause'
        if (pause) {
            // Si en pause : Fond Rouge (RED) en mode normal
            GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(DARKGRAY));
            GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, ColorToInt(BLACK)); // Survolé
        }

        // 2. Dessiner le bouton (il appliquera automatiquement les couleurs définies au-dessus)
        if (GuiButton((Rectangle){20, 20, 40, 40}, GuiIconText(ICON_AUDIO, ""))) {
            pause = !pause;
            if (pause) PauseMusicStream(music);
            else ResumeMusicStream(music);
        }

        // 3. IMPORTANT : Remettre la couleur par défaut après le bouton 
        // pour ne pas modifier TOUS les autres boutons du jeu !
        GuiLoadStyleDefault();

        // Remettre à la taille normale (1) pour la suite du code
        GuiSetIconScale(1);

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

    save_progress(&game);

    UnloadTexture(background);  // Unload background texture
    UnloadTexture(midground);   // Unload midground texture
    UnloadTexture(foreground);  // Unload foreground texture

    // 4. Nettoyage et fermeture du contexte GPU
    CloseWindow();
    return 0;
}