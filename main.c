#include "raylib.h"
#include "stdio.h"

#include "background.h"
#include "player.h"

// gcc main.c player.c background.c -g -o main.exe -lraylib -lopengl32 -lgdi32 -lwinmm
// ./main.exe

int main(void)
{
    // Virtual game resolution
    const int gameWidth = 320;
    const int gameHeight = 224;

    // Create a resizable 4:3 window
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 600, "Windjammer_lite");
    SetTargetFPS(60);

    // Render the game at a fixed resolution
    RenderTexture2D target = LoadRenderTexture(gameWidth, gameHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);

    // Create background
    Background background;
    BackgroundInit(&background);

    BackgroundAddFrame(&background, "assets/Sandmap/wjammers001.png");
    BackgroundAddFrame(&background, "assets/Sandmap/wjammers002.png");
    BackgroundAddFrame(&background, "assets/Sandmap/wjammers003.png");
    BackgroundAddFrame(&background, "assets/Sandmap/wjammers004.png");
    BackgroundAddObject(&background, "assets/Sandmap/Map_net.png", (Vector2){8, 0});
    

    // Create player
    Player player;
    PlayerInit(&player, (Vector2){50, 120});
    PlayerSetTexture(&player, "assets/Girl1_movement.png");
    

    while (!WindowShouldClose())
    {
        // Update logic
        BackgroundUpdate(&background);
        PlayerUpdate(&player);
       // printf("Position: X=%.2f Y=%.2f | Velocity: X=%.2f Y=%.2f\n", player.position.x, player.position.y, player.velocity.x, player.velocity.y); //debug

        // Draw game to the fixed-resolution texture
        BeginTextureMode(target);
            ClearBackground(BLACK);
            BackgroundDraw(&background);
            PlayerDraw(&player);
        EndTextureMode();

        // Draw the scaled game to the window
        BeginDrawing();

            ClearBackground(BLACK);

            float scaleX = (float)GetScreenWidth() / gameWidth;
            float scaleY = (float)GetScreenHeight() / gameHeight;
            float scale = scaleX;

            // Use the smaller scale to keep the 4:3 aspect ratio
            if (scaleY < scale)
                scale = scaleY;

            float width = gameWidth * scale;
            float height = gameHeight * scale;

            Rectangle source = {0, 0, (float)target.texture.width, -(float)target.texture.height};

            Rectangle dest = {(GetScreenWidth() - width) / 2, (GetScreenHeight() - height) / 2, width, height};

            DrawTexturePro(target.texture, source, dest, (Vector2){0, 0}, 0, WHITE);

        EndDrawing();
    }

    // Clean up
    PlayerUnload(&player);
    UnloadRenderTexture(target);
    CloseWindow();

    return 0;
}