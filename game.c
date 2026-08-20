#include "game.h"
#include "raylib.h"

static void GameUpdate(Game *game);
static void GameDraw(Game *game);

static void GameUpdateGameplay(Game *game);
static void GameDrawGameplay(Game *game);

static void GameDrawMenu(Game *game);


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
        game->disk.position = game->player.position;

        DiskThrow(&game->disk, throwDirection);
    }

    DiskUpdate(&game->disk);
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