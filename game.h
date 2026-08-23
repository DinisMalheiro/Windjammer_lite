#ifndef GAME_H
#define GAME_H

#include "raylib.h"

#include "background.h"
#include "player.h"
#include "disk.h"
#include "cpu.h"

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
    int gameWidth;
    int gameHeight;

    GameScreen screen;

    RenderTexture2D target;

    Background background;
    Player player;
    CPU cpu;
    Disk disk;

    Rectangle playerArea;
    Rectangle cpuArea;
    Rectangle diskArena;

    Goal leftGoal;
    Goal rightGoal;

    int leftScore;
    int rightScore;

    float gameTime;
    float gameTimer;
} Game;

void GameInit(Game *game);
void GameRun(Game *game);
void GameUnload(Game *game);

#endif