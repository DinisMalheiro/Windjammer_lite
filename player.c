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

    player->facingDirection = (Vector2){1.0f, 0.0f};

    player->speed = 150.0f;

    player->hasDisk = true;

    player->side = PLAYER_SIDE_LEFT;

    player->throwAngle = 30.0f;

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
    player->animation = -1;

    PlayerSetAnimation(player, PLAYER_ANIM_IDLE);
}

// Update player movement and animation
void PlayerUpdate(Player *player)
{
    float deltaTime = GetFrameTime();


    // Cannot move while holding disk
    if (player->hasDisk)
    {
        player->velocity = (Vector2){0};

        PlayerSetAnimation(player, PLAYER_ANIM_IDLE);

        // Still animate while holding the disk
        player->animationTimer += deltaTime;

        if (player->animationTimer >= player->frameTime)
        {
            player->animationTimer -= player->frameTime;
            player->frame++;

            if (player->frame >= player->columns)
                player->frame = 0;
        }

        return;
    }

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

    bool isMoving =
        player->velocity.x != 0.0f ||
        player->velocity.y != 0.0f;

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

        // Remember the direction the player was moving
        player->facingDirection = player->velocity;

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
        if (player->velocity.x > 0.0f)
            PlayerSetAnimation(player, PLAYER_ANIM_WALK_RIGHT);
        else
            PlayerSetAnimation(player, PLAYER_ANIM_WALK_LEFT);
    }
    else
    {
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
            player->playerSprite, sourceRec, destRec, origin, 0.0f, WHITE);
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

//map collisions
void PlayerUpdateMapLimits(Player *player, Rectangle area)
{
    float halfWidth = player->frameWidth / 2.0f;
    float halfHeight = player->frameHeight / 2.0f;

    if (player->position.x < area.x + halfWidth)
        player->position.x = area.x + halfWidth;

    if (player->position.x > area.x + area.width - halfWidth)
        player->position.x = area.x + area.width - halfWidth;

    if (player->position.y < area.y + halfHeight)
        player->position.y = area.y + halfHeight;

    if (player->position.y > area.y + area.height - halfHeight)
        player->position.y = area.y + area.height - halfHeight;
}

Vector2 PlayerGetThrowDirection(const Player *player)
{
    Vector2 forward;

    // Player on left side throws toward the right
    if (player->side == PLAYER_SIDE_LEFT)
    {
        forward = (Vector2){1.0f, 0.0f};
    }
    // Player on right side throws toward the left
    else
    {
        forward = (Vector2){-1.0f, 0.0f};
    }

    float angle = 0.0f;

    /*
     * W = throw upward
     * S = throw downward
     *
     * A/D do not allow backward throws.
     */
    if (IsKeyDown(KEY_W))
    {
        angle = -player->throwAngle;
    }
    else if (IsKeyDown(KEY_S))
    {
        angle = player->throwAngle;
    }

    float radians = angle * DEG2RAD;

    Vector2 direction;

    direction.x =
        forward.x * cosf(radians) -
        forward.y * sinf(radians);

    direction.y =
        forward.x * sinf(radians) +
        forward.y * cosf(radians);

    return direction;
}

