#include <stdio.h>
#include <teste.h>

void teste(const GameContext *ctx, const GameSave *save) {
    int i, j;

    printf("===== GAME CONTEXT =====\n");

    printf("Operator : %s\n", ctx->operator_name);
    printf("Level    : %d\n", ctx->current_level_idx);
    printf("Surcharge: %d\n", ctx->surcharge);

    printf("Cursor   : (%d,%d)\n", ctx->cursor_x, ctx->cursor_y);
    printf("Selected : (%d,%d)\n", ctx->selected_x, ctx->selected_y);

    printf("State    : %d\n", ctx->state);

    printf("\n--- Matrix ---\n");
    for (i = 0; i < MAP_ROWS; i++) {
        for (j = 0; j < MAP_COLS; j++) {
            printf("%d ", ctx->matrix[i][j]);
        }
        printf("\n");
    }

    printf("\n--- Marked ---\n");
    for (i = 0; i < MAP_ROWS; i++) {
        for (j = 0; j < MAP_COLS; j++) {
            printf("%d ", ctx->marked[i][j]);
        }
        printf("\n");
    }

    printf("\n--- Level Config ---\n");

    printf("Max moves    : %d\n", ctx->level.max_moves);
    printf("Current moves: %d\n", ctx->level.cur_moves);
    printf("Time limit   : %.2f\n", ctx->level.time_limit);
    printf("Time elapsed : %.2f\n", ctx->level.time_elapsed);

    printf("Required energy:\n");
    for (i = 0; i < CELL_COUNT; i++) {
        printf("  req_energy[%d] = %d\n",
               i,
               ctx->level.req_energy[i]);
    }

    printf("Current energy:\n");
    for (i = 0; i < CELL_COUNT; i++) {
        printf("  cur_energy[%d] = %d\n",
               i,
               ctx->level.cur_energy[i]);
    }

    printf("\n===== SAVE SLOTS =====\n");

    printf("Slot 1:\n");
    printf("  Name      : %s\n", save->operator_name_1);
    printf("  Level     : %d\n", save->current_level_idx_1);
    printf("  Surcharge : %d\n", save->surcharge_1);

    printf("Slot 2:\n");
    printf("  Name      : %s\n", save->operator_name_2);
    printf("  Level     : %d\n", save->current_level_idx_2);
    printf("  Surcharge : %d\n", save->surcharge_2);

    printf("Slot 3:\n");
    printf("  Name      : %s\n", save->operator_name_3);
    printf("  Level     : %d\n", save->current_level_idx_3);
    printf("  Surcharge : %d\n", save->surcharge_3);

    printf("=======================\n");
}