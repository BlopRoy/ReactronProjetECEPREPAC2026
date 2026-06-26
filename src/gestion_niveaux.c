#include "gestion_niveaux.h"
#include <string.h>

void load_level_config(GameContext *ctx, int level_num) {
    ctx->current_level_idx = level_num;
    ctx->level.cur_moves = 0;
    ctx->level.time_elapsed = 0.0f;
    
    for (int i = 0; i < CELL_COUNT; i++) {
        ctx->level.req_energy[i] = 0;
        ctx->level.cur_energy[i] = 0;
    }

    switch (level_num) {
        case 1:
            ctx->level.req_energy[CELL_R] = 15;
            ctx->level.req_energy[CELL_B] = 15;
            ctx->level.max_moves = 30;
            ctx->level.time_limit = 180.0f;
            break;
            
        case 2:
            ctx->level.req_energy[CELL_R] = 20;
            ctx->level.req_energy[CELL_G] = 20;
            ctx->level.req_energy[CELL_Y] = 15;
            ctx->level.max_moves = 25;
            ctx->level.time_limit = 150.0f;
            break;
            
        case 3:
        default:
            ctx->level.req_energy[CELL_R] = 25;
            ctx->level.req_energy[CELL_B] = 25;
            ctx->level.req_energy[CELL_G] = 25;
            ctx->level.req_energy[CELL_Y] = 25;
            ctx->level.req_energy[CELL_V] = 20;
            ctx->level.max_moves = 35;
            ctx->level.time_limit = 120.0f;
            break;
    }
}

void check_game_conditions(GameContext *ctx) {
    bool victory = true;
    for (int i = 1; i < CELL_COUNT; i++) {
        if (ctx->level.req_energy[i] > 0 && ctx->level.cur_energy[i] < ctx->level.req_energy[i]) {
            victory = false;
            break;
        }
    }

    if (victory && ctx->level.cur_moves <= ctx->level.max_moves && ctx->level.time_elapsed <= ctx->level.time_limit) {
        ctx->state = STATE_VICTORY;
        return;
    }

    if (ctx->level.cur_moves >= ctx->level.max_moves || ctx->level.time_elapsed >= ctx->level.time_limit) {
        ctx->state = STATE_GAMEOVER;
    }
}

void handle_level_fail(GameContext *ctx) {
    ctx->surcharge++;

    if (ctx->surcharge >= MAX_SURCHARGE) {
        ctx->state = STATE_CLEAR;
    }
}
