#ifndef SAUVEGARDE_H
#define SAUVEGARDE_H

#include <stdbool.h>
#include "type.h"

bool save_progress(GameContext *ctx);

bool load_progress(GameContext *ctx);

void init_save(GameContext *ctx);

#endif
