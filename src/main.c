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
    srand((unsigned int)time(NULL));

    const int screenWidth = 1100;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Reactron - Quantum Reactor Stabilizer");
    Texture2D background = LoadTexture("assets/cyberpunk_street_background.png");
    Texture2D midground = LoadTexture("assets/cyberpunk_street_midground.png");
    Texture2D foreground = LoadTexture("assets/cyberpunk_street_foreground.png");
    InitAudioDevice();

    Music music = LoadMusicStream("assets/bgm_old.mp3");
    PlayMusicStream(music);
    bool pause = false;

    SetTargetFPS(60);

    if (!IsWindowReady()) {
        TraceLog(LOG_ERROR, "Impossible d'initialiser le moteur graphique Raylib. Fermeture du programme.");
        return -1; 
    }

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

    float cascade_timer = 0.0f;
    const float CASCADE_DELAY = 0.15f;

    while (!WindowShouldClose()) {
        UpdateMusicStream(music);
        
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

                if (IsKeyPressed(KEY_UP) && game.cursor_y > 0) game.cursor_y--; 
                if (IsKeyPressed(KEY_DOWN) && game.cursor_y < MAP_ROWS - 1) game.cursor_y++; 
                if (IsKeyPressed(KEY_LEFT) && game.cursor_x > 0) game.cursor_x--;
                if (IsKeyPressed(KEY_RIGHT) && game.cursor_x < MAP_COLS - 1) game.cursor_x++;

                if (IsKeyPressed(KEY_SPACE)) {
                    if (game.selected_x == -1) {
                        game.selected_x = game.cursor_x;
                        game.selected_y = game.cursor_y;
                    } else {
                        int dist = abs(game.selected_x - game.cursor_x) + abs(game.selected_y - game.cursor_y);
                        if (dist == 1) {
                            if (swap_cells(&game, game.selected_x, game.selected_y, game.cursor_x, game.cursor_y)) {
                                game.level.cur_moves++;
                                game.state = STATE_CASCADE;
                            }
                        }
                        game.selected_x = -1;
                    }
                    teste(&game);
                }
                break;

            case STATE_CASCADE:
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
                        load_level_config(&game, game.current_level_idx);
                        init_matrix(&game);
                        game.state = STATE_GAMEPLAY;
                    }
                }
                break;
                
            case STATE_CLEAR:
                if (IsKeyPressed(KEY_ENTER)) { 
                    game.surcharge = 0;
                    game.current_level_idx = 1;
                    load_level_config(&game, 1);
                    init_matrix(&game);
                    save_progress(&game);
                    game.state = STATE_GAMEPLAY;
                }
                break;
        }

        BeginDrawing();
        DrawMovingBackground(&backg,background,midground,foreground);
        GuiSetIconScale(2);

        if (game.state != STATE_MENU) {
            if (GuiButton((Rectangle){75, 20, 40, 40}, GuiIconText(ICON_HOUSE, ""))) {
        
                if (game.state == STATE_GAMEPLAY || game.state == STATE_CASCADE) {
                    save_progress(&game); 
                }
        
                game.state = STATE_MENU;
            }
        }


        if (pause) {
            GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(DARKGRAY));
            GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, ColorToInt(BLACK)); // Survolé
        }

        if (GuiButton((Rectangle){20, 20, 40, 40}, GuiIconText(ICON_AUDIO, ""))) {
            pause = !pause;
            if (pause) PauseMusicStream(music);
            else ResumeMusicStream(music);
        }

        GuiLoadStyleDefault();

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
                DrawGridi(&game);
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

    UnloadTexture(background);
    UnloadTexture(midground);
    UnloadTexture(foreground);

    CloseWindow();
    return 0;
}
