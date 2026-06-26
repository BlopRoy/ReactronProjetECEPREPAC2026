#ifndef TYPES_H
#define TYPES_H

#include "raylib.h"

#define MAP_ROWS 12 
#define MAP_COLS 20 

#define MAX_SURCHARGE 5 

typedef enum {
    CELL_EMPTY = 0,
    CELL_R,
    CELL_B,
    CELL_G,
    CELL_Y,
    CELL_V,
    CELL_COUNT
} CellType;

typedef enum {
    STATE_MENU,
    STATE_GAMEPLAY,
    STATE_CASCADE,
    STATE_VICTORY,
    STATE_GAMEOVER,
    STATE_CLEAR
} GameState;

typedef struct{
    float scrollingBack;
    float scrollingMid;
    float scrollingFore;
} ScrollingBackground;

typedef struct {
    int req_energy[CELL_COUNT];
    int cur_energy[CELL_COUNT];
    int max_moves;
    int cur_moves;
    float time_limit;
    float time_elapsed;
} LevelConfig;

typedef struct {
    char operator_name_1[50];
    char operator_name_2[50];
    char operator_name_3[50];
    int surcharge_1;
    int surcharge_2;
    int surcharge_3;
    int current_level_idx_1;
    int current_level_idx_2;
    int current_level_idx_3;
} GameSave;

typedef struct {
    CellType matrix[MAP_ROWS][MAP_COLS];
    int marked[MAP_ROWS][MAP_COLS];
    
    int cursor_x;
    int cursor_y;
    
    int selected_x;
    int selected_y;
    
    int surcharge;
    int current_level_idx;
    char operator_name[50];
    
    LevelConfig level;
    GameState state;
    GameSave save;
} GameContext;

#endif
