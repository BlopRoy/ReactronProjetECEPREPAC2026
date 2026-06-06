#ifndef DETECTION_ALIGNEMENTS_H
#define DETECTION_ALIGNEMENTS_H

#include <stdbool.h>
#include "type.h"

bool detect_and_mark(GameContext *ctx);
int resolve_suppression(GameContext *ctx);

#endif