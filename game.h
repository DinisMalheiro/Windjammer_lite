#ifndef GAME_H
#define GAME_H

#include "raylib.h"

#include "background.h"
#include "player.h"
#include "disk.h"

typedef enum GameScreen
{
    GAME_SCREEN_MENU,
    GAME_SCREEN_GAMEPLAY
} GameScreen;

typedef struct Goal
{
    float x;

    float top;
    float zone1Bottom;
    float zone2Bottom;
    float bottom;

} Goal;

typedef struct Game
{
    // Virtual resolution
    int gameWidth;
    int gameHeight;

    // Current screen
    GameScreen screen;

    // Game render target
    RenderTexture2D target;

    // Game objects
    Background background;
    Player player;
    Disk disk;

    // Player movement area
    Rectangle playerArea;
    
    // Disk collision area
    Rectangle diskArena;

    Goal leftGoal;
    Goal rightGoal;

} Game;

void GameInit(Game *game);
void GameRun(Game *game);
void GameUnload(Game *game);

#endif