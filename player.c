#include "player.h"
#include <math.h>

// Change the current animation
static void PlayerSetAnimation(Player *player, PlayerAnimation animation)
{
    if (player->animation == animation)
        return;

    player->animation = animation;
    player->frame = 0;
    player->animationTimer = 0.0f;

    switch (animation)
    {
        case PLAYER_ANIM_IDLE:
            player->currentRow = 4;
            break;

        case PLAYER_ANIM_WALK_RIGHT:
            player->currentRow = 0;
            break;

        case PLAYER_ANIM_WALK_LEFT:
            player->currentRow = 1;
            break;

        case PLAYER_ANIM_WALK_DOWN:
            player->currentRow = 2;
            break;

        case PLAYER_ANIM_WALK_UP:
            player->currentRow = 3;
            break;
    }
}

// Initialize player
void PlayerInit(Player *player, Vector2 position)
{
    player->position = position;
    player->velocity = (Vector2){0};
    player->speed = 150.0f;
    player->playerSprite = (Texture2D){0};

    // Sprite sheet
    player->frame = 0;
    player->currentRow = 5;
    player->columns = 5;
    player->rows = 6;
    player->frameWidth = 64;
    player->frameHeight = 64;

    // Animation
    player->animationTimer = 0.0f;
    player->frameTime = 0.12f;
    player->animation = -1; //reset animation when starting
    PlayerSetAnimation(player, PLAYER_ANIM_IDLE);
}

// Update player movement and animation
void PlayerUpdate(Player *player)
{
    float deltaTime = GetFrameTime();

    // Read input
    player->velocity = (Vector2){0};

    if (IsKeyDown(KEY_W))
        player->velocity.y -= 1.0f;

    if (IsKeyDown(KEY_S))
        player->velocity.y += 1.0f;

    if (IsKeyDown(KEY_A))
        player->velocity.x -= 1.0f;

    if (IsKeyDown(KEY_D))
        player->velocity.x += 1.0f;

    // Check if player is moving
    bool isMoving =
        player->velocity.x != 0.0f ||
        player->velocity.y != 0.0f;

    // Normalize diagonal movement
    if (isMoving)
    {
        float length = sqrtf(
            player->velocity.x * player->velocity.x +
            player->velocity.y * player->velocity.y
        );

        if (length > 0.0f)
        {
            player->velocity.x /= length;
            player->velocity.y /= length;
        }

        // Move player
        player->position.x +=
            player->velocity.x * player->speed * deltaTime;

        player->position.y +=
            player->velocity.y * player->speed * deltaTime;
    }

    // Select animation
    if (!isMoving)
    {
        PlayerSetAnimation(player, PLAYER_ANIM_IDLE);
    }
    else if (fabsf(player->velocity.x) > fabsf(player->velocity.y))
    {
        // Horizontal movement
        if (player->velocity.x > 0.0f)
            PlayerSetAnimation(player, PLAYER_ANIM_WALK_RIGHT);
        else
            PlayerSetAnimation(player, PLAYER_ANIM_WALK_LEFT);
    }
    else
    {
        // Vertical movement
        if (player->velocity.y > 0.0f)
            PlayerSetAnimation(player, PLAYER_ANIM_WALK_DOWN);
        else
            PlayerSetAnimation(player, PLAYER_ANIM_WALK_UP);
    }

    // Update animation
    player->animationTimer += deltaTime;

    if (player->animationTimer >= player->frameTime)
    {
        player->animationTimer -= player->frameTime;
        player->frame++;

        if (player->frame >= player->columns)
            player->frame = 0;
    }
}

// Draw player
void PlayerDraw(const Player *player)
{
    if (player->playerSprite.id != 0)
    {
        Rectangle sourceRec = {
            (float)(player->frame * player->frameWidth),
            (float)(player->currentRow * player->frameHeight),
            (float)player->frameWidth,
            (float)player->frameHeight
        };

        Rectangle destRec = {
            player->position.x,
            player->position.y,
            (float)player->frameWidth,
            (float)player->frameHeight
        };

        Vector2 origin = {
            (float)player->frameWidth / 2.0f,
            (float)player->frameHeight / 2.0f
        };

        DrawTexturePro(
            player->playerSprite,
            sourceRec,
            destRec,
            origin,
            0.0f,
            WHITE
        );
    }
    else
    {
        // Draw a circle if no texture is loaded
        DrawCircleV(player->position, 20.0f, BLUE);
    }
}

// Unload player resources
void PlayerUnload(Player *player)
{
    if (player->playerSprite.id != 0)
    {
        UnloadTexture(player->playerSprite);
        player->playerSprite = (Texture2D){0};
    }
}

// Load player texture
void PlayerSetTexture(Player *player, const char *texturePath)
{
    if (player->playerSprite.id != 0)
        UnloadTexture(player->playerSprite);

    player->playerSprite = LoadTexture(texturePath);
}