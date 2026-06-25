#ifndef RENDERING_H
#define RENDERING_H

#include "raylib.h"
#include "type.h"

Color GetCellColor(CellType type);
void DrawGridi(const GameContext *ctx);
void DrawInterface(const GameContext *ctx);

// Écrans de la machine à états
void DrawMenuScreen(GameContext *ctx);
void DrawVictoryScreen(const GameContext *ctx);
void DrawGameOverScreen(const GameContext *ctx);
void DrawDefinitiveInstabilityScreen(const GameContext *ctx);
void DrawMovingBackground(ScrollingBackground *b,Texture2D background,Texture2D midground, Texture2D foreground);

#endif