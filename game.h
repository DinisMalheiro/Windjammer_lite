#ifndef GAME_H
#define GAME_H
#include "menu.h"
#include "sound.h"

#include "raylib.h"
#include "assets.h"
#include "background.h"
#include "player.h"
#include "disk.h"
#include "cpu.h"

typedef enum GameScreen
{
    GAME_SCREEN_MENU,
    GAME_SCREEN_GAMEPLAY,
    GAME_SCREEN_PAUSE
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
    // Game resolution
    int gameWidth;
    int gameHeight;
    
    // Main menu
    Menu menu;

    // Current screen
    GameScreen screen;
    float shakeTimer;
    float shakeDuration;
    float shakeStrength;

    // Render target
    RenderTexture2D target;

    // Shared game assets
    GameAssets assets;

    // Game objects
    Background background;
    Player player;
    CPU cpu;
    Disk disk;

    // Gameplay areas
    Rectangle playerArea;
    Rectangle cpuArea;
    Rectangle diskArena;

    // Audio
    SoundManager sound;

    // Sound effect IDs
    int soundMenuMove;
    int soundMenuSelect;
    int soundThrow;
    int soundDiskHitwall;
    int sound3Points;
    int sound5Points;

    // Goals
    Goal leftGoal;
    Goal rightGoal;

    // Score
    int leftScore;
    int rightScore;

    // Game timer
    float gameTime;
    float gameTimer;
} Game;

void GameInit(Game *game);
void GameRun(Game *game);
void GameUnload(Game *game);
void GameReset(Game *game);

#endif