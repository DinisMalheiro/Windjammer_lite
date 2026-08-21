#include "game.h"
#include "raylib.h"
#include <math.h>

static void GameUpdate(Game *game);
static void GameDraw(Game *game);

static void GameUpdateGameplay(Game *game);
static void GameDrawGameplay(Game *game);

static void GameDrawMenu(Game *game);

static void GameUpdateDiskCollision(Game *game);
static int GameGetGoalZone(const Goal *goal, Vector2 diskPosition);
static void GameHandleGoal(Game *game, int side, int zone);

static bool GameTryCatchDisk(Game *game);


void GameInit(Game *game)
{
    game->gameWidth = 320;
    game->gameHeight = 224;

    game->screen = GAME_SCREEN_GAMEPLAY;

    // Window
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 600, "Windjammer_lite");
    SetTargetFPS(60);

    // Render target
    game->target = LoadRenderTexture(game->gameWidth, game->gameHeight);
    SetTextureFilter(game->target.texture, TEXTURE_FILTER_POINT);

    // Background
    BackgroundInit(&game->background);

    BackgroundAddFrame(&game->background, "assets/Sandmap/wjammers001.png");
    BackgroundAddFrame(&game->background, "assets/Sandmap/wjammers002.png");
    BackgroundAddFrame(&game->background, "assets/Sandmap/wjammers003.png");
    BackgroundAddFrame(&game->background, "assets/Sandmap/wjammers004.png");

    BackgroundAddObject(&game->background, "assets/Sandmap/Map_net.png", (Vector2){8, 0});

    // Player
    PlayerInit(&game->player, (Vector2){50, 120});
    PlayerSetTexture(&game->player, "assets/Girl1_movement.png");

    game->playerArea = (Rectangle){0, 30, game->gameWidth / 2 + 10, game->gameHeight - 48};

    // Disk
    DiskInit(&game->disk, game->player.position);

    game->disk.held = true;
    game->disk.thrown = false;


     // Disk collision area
    game->diskArena = (Rectangle){0, 30, 320, 146};

    // Goal X coordinates
    game->leftGoal.x = 0;
    game->rightGoal.x = 320;

    // Goal Y zones
    // Zone 1: 30 - 70
    // Zone 2: 70 - 136
    // Zone 3: 136 - 176
    // Zone 2 is intentionally larger.

    game->leftGoal.top = 30;
    game->leftGoal.zone1Bottom = 70;
    game->leftGoal.zone2Bottom = 136;
    game->leftGoal.bottom = 176;

    game->rightGoal.top = 30;
    game->rightGoal.zone1Bottom = 70;
    game->rightGoal.zone2Bottom = 136;
    game->rightGoal.bottom = 176;

}


void GameRun(Game *game)
{
    while (!WindowShouldClose())
    {
        GameUpdate(game);
        GameDraw(game);
    }
}


static void GameUpdate(Game *game)
{
    switch (game->screen)
    {
        case GAME_SCREEN_MENU:
            // MenuUpdate(game);
            break;

        case GAME_SCREEN_GAMEPLAY:
            GameUpdateGameplay(game);
            break;
    }
}


static void GameUpdateGameplay(Game *game)
{
    BackgroundUpdate(&game->background);

    PlayerUpdate(&game->player);
    PlayerUpdateMapLimits(&game->player, game->playerArea);

    // DEBUG: X gives player the disk
    if (IsKeyPressed(KEY_X))
    {
        game->player.hasDisk = true;

        game->disk.held = true;
        game->disk.thrown = false;
        game->disk.velocity = (Vector2){0};
        game->disk.position = game->player.position;
    }

    // Throw disk
    if (game->player.hasDisk && IsKeyPressed(KEY_SPACE))
    {
        Vector2 throwDirection = PlayerGetThrowDirection(&game->player);

        game->player.hasDisk = false;
        game->player.canMove = false;
        game->player.throwTimer = game->player.throwDelay;

        game->disk.position = game->player.position;

        DiskThrow(&game->disk, throwDirection);
    }

    DiskUpdate(&game->disk);
    GameUpdateDiskCollision(game);

    if (!game->player.hasDisk)
        GameTryCatchDisk(game);
}


static bool GameTryCatchDisk(Game *game)
{
    Disk *disk = &game->disk;
    Player *player = &game->player;

    if (!disk->thrown || disk->catchTimer > 0.0f)
        return false;

    if (!DiskPlayerCollision(disk, player->position, player->catchRadius))
        return false;

    disk->position = player->position;
    disk->velocity = (Vector2){0};
    disk->thrown = false;
    disk->held = true;

    player->hasDisk = true;

    return true;
}

static void GameUpdateDiskCollision(Game *game)
{
    Disk *disk = &game->disk;
    Rectangle arena = game->diskArena;

    if (!disk->thrown)
        return;

    const float radius = 6.0f;

    // Top wall
    if (disk->position.y - radius <= arena.y)
    {
        disk->position.y = arena.y + radius;
        disk->velocity.y = fabsf(disk->velocity.y);
    }

    // Bottom wall
    if (disk->position.y + radius >= arena.y + arena.height)
    {
        disk->position.y = arena.y + arena.height - radius;
        disk->velocity.y = -fabsf(disk->velocity.y);
    }

    // Left side
    if (disk->position.x <= game->leftGoal.x)
    {
        int zone = GameGetGoalZone(&game->leftGoal, disk->position);

        if (zone != 0)
        {
            GameHandleGoal(game, -1, zone);
            return;
        }

        disk->position.x = game->leftGoal.x + radius;
        disk->velocity = (Vector2){0};
        disk->thrown = false;
    }

    // Right side
    if (disk->position.x >= game->rightGoal.x)
    {
        int zone = GameGetGoalZone(&game->rightGoal, disk->position);

        if (zone != 0)
        {
            GameHandleGoal(game, 1, zone);
            return;
        }

        disk->position.x = game->rightGoal.x - radius;
        disk->velocity = (Vector2){0};
        disk->thrown = false;
    }
}

static int GameGetGoalZone(const Goal *goal, Vector2 diskPosition)
{
    if (diskPosition.y < goal->top)
        return 0;

    if (diskPosition.y < goal->zone1Bottom)
        return 1;

    if (diskPosition.y < goal->zone2Bottom)
        return 2;

    if (diskPosition.y <= goal->bottom)
        return 3;

    return 0;
}

static void GameHandleGoal(Game *game, int side, int zone)
{ //debug
    if (side > 0)
    {
        TraceLog(LOG_INFO, "RIGHT GOAL - ZONE %d", zone);

        game->disk.position.x = game->rightGoal.x - 6.0f;
        game->disk.velocity.x = -fabsf(game->disk.velocity.x);
    }
    else
    {
        TraceLog(LOG_INFO, "LEFT GOAL - ZONE %d", zone);

        game->disk.velocity = (Vector2){0};
        game->disk.thrown = false;

        game->player.hasDisk = true;
        game->player.canMove = false;
        game->disk.held = true;
        game->disk.position = game->player.position;
    }
    /*
    if (side < 0)
        TraceLog(LOG_INFO, "LEFT GOAL - ZONE %d", zone);
    else
        TraceLog(LOG_INFO, "RIGHT GOAL - ZONE %d", zone);

    game->disk.velocity = (Vector2){0};
    game->disk.thrown = false;

    game->player.hasDisk = true;
    game->disk.held = true;
    game->disk.position = game->player.position;
    */
}

static void GameDraw(Game *game)
{
    // Draw game to fixed-resolution render target
    BeginTextureMode(game->target);

        ClearBackground(BLACK);

        switch (game->screen)
        {
            case GAME_SCREEN_MENU:
                GameDrawMenu(game);
                break;

            case GAME_SCREEN_GAMEPLAY:
                GameDrawGameplay(game);
                break;
        }

    EndTextureMode();


    // Draw render target scaled to window
    BeginDrawing();

        ClearBackground(BLACK);

        float scaleX = (float)GetScreenWidth() / game->gameWidth;
        float scaleY = (float)GetScreenHeight() / game->gameHeight;
        float scale = scaleX;

        if (scaleY < scale)
            scale = scaleY;

        float width = game->gameWidth * scale;
        float height = game->gameHeight * scale;

        Rectangle source = {0, 0, (float)game->target.texture.width, -(float)game->target.texture.height};

        Rectangle dest = {(GetScreenWidth() - width) / 2, (GetScreenHeight() - height) / 2, width, height};

        DrawTexturePro(game->target.texture, source, dest, (Vector2){0, 0}, 0, WHITE);

    EndDrawing();
}


static void GameDrawGameplay(Game *game)
{
    BackgroundDraw(&game->background);
    DiskDraw(&game->disk);
    PlayerDraw(&game->player);
}


static void GameDrawMenu(Game *game)
{
    DrawText("WINDJAMMER LITE", 60, 70, 24, WHITE);
    DrawText("PRESS ENTER", 100, 130, 16, WHITE);
}


void GameUnload(Game *game)
{
    DiskUnload(&game->disk);
    PlayerUnload(&game->player);

    UnloadRenderTexture(game->target);

    CloseWindow();
}