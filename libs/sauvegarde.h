#ifndef SAUVEGARDE_H
#define SAUVEGARDE_H

#include <stdbool.h>
#include "type.h"

// Sauvegarde les informations importantes de la partie dans un fichier
bool save_progress(GameContext *ctx);

// Charge les informations sauvegardées pour reprendre la partie
bool load_progress(GameContext *ctx);

void init_save(GameContext *ctx);

#endif
