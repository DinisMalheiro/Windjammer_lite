#include "player.h"
#include <math.h>

static void PlayerSetAnimation(Player *player, PlayerAnimation animation)
{
    if (player->animation == animation)
        return;

    player->animation = animation;
    player->frame = 0;
    player->animationTimer = 0.0f;

    switch (animation)
    {
        case PLAYER_ANIM_IDLE: player->currentRow = 4; break;
        case PLAYER_ANIM_WALK_RIGHT: player->currentRow = 0; break;
        case PLAYER_ANIM_WALK_LEFT: player->currentRow = 1; break;
        case PLAYER_ANIM_WALK_DOWN: player->currentRow = 2; break;
        case PLAYER_ANIM_WALK_UP: player->currentRow = 3; break;
    }
}

void PlayerInit(Player *player, Vector2 position)
{
    *player = (Player){0};

    player->position = position;
    player->facingDirection = (Vector2){1.0f, 0.0f};
    player->speed = 150.0f;
    player->hasDisk = true;
    player->side = PLAYER_SIDE_LEFT;

    // Throw settings
    player->throwAngle = 60.0f;
    player->throwTimer = 0.0f;
    player->throwDelay = 0.2f;

    // Catch settings
    player->catchRadius = 20.0f;

    // Player starts locked while holding the disk
    player->canMove = false;

    // Sprite sheet
    player->frame = 0;
    player->currentRow = 4;
    player->columns = 5;
    player->rows = 6;
    player->frameWidth = 64;
    player->frameHeight = 64;

    // Animation
    player->animationTimer = 0.0f;
    player->frameTime = 0.12f;
    player->animation = PLAYER_ANIM_IDLE;
}

void PlayerUpdate(Player *player)
{
    float deltaTime = GetFrameTime();

    // Player cannot move while holding the disk
    if (player->hasDisk)
    {
        player->velocity = (Vector2){0};
        PlayerSetAnimation(player, PLAYER_ANIM_IDLE);

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

    // Short delay after throwing
    if (player->throwTimer > 0.0f)
    {
        player->throwTimer -= deltaTime;

        if (player->throwTimer <= 0.0f)
        {
            player->throwTimer = 0.0f;
            player->canMove = true;
        }
    }

    // Player is locked immediately after throwing
    if (!player->canMove)
    {
        player->velocity = (Vector2){0};
        PlayerSetAnimation(player, PLAYER_ANIM_IDLE);

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

    // Read movement input
    player->velocity = (Vector2){0};

    if (IsKeyDown(KEY_W))
        player->velocity.y -= 1.0f;

    if (IsKeyDown(KEY_S))
        player->velocity.y += 1.0f;

    if (IsKeyDown(KEY_A))
        player->velocity.x -= 1.0f;

    if (IsKeyDown(KEY_D))
        player->velocity.x += 1.0f;

    bool isMoving = player->velocity.x != 0.0f || player->velocity.y != 0.0f;

    if (isMoving)
    {
        // Normalize movement so diagonal movement is not faster
        float length = sqrtf(player->velocity.x * player->velocity.x + player->velocity.y * player->velocity.y);

        if (length > 0.0f)
        {
            player->velocity.x /= length;
            player->velocity.y /= length;
        }

        // Remember the direction the player is facing
        player->facingDirection = player->velocity;

        // Move player
        player->position.x += player->velocity.x * player->speed * deltaTime;
        player->position.y += player->velocity.y * player->speed * deltaTime;
    }

    // Select animation based on movement direction
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

void PlayerDraw(const Player *player)
{
    if (player->sprite != 0)
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
            player->frameWidth / 2.0f,
            player->frameHeight / 2.0f
        };

        DrawTexturePro(*player->sprite, sourceRec, destRec, origin, 0.0f, WHITE);
    }
    else
    {
        // Fallback when no sprite is assigned
        DrawCircleV(player->position, 20.0f, BLUE);
    }
}

void PlayerSetTexture(Player *player, Texture2D *texture)
{
    // Player only stores a pointer. GameAssets owns the texture.
    player->sprite = texture;
}

void PlayerUnload(Player *player)
{
    // Do NOT unload the texture here.
    // The texture belongs to GameAssets.
    player->sprite = 0;
}

void PlayerUpdateMapLimits(Player *player, Rectangle area)
{
    float halfWidth = player->frameWidth / 2.0f;
    float halfHeight = player->frameHeight / 2.0f;

    // Left boundary
    if (player->position.x < area.x + halfWidth)
        player->position.x = area.x + halfWidth;

    // Right boundary
    if (player->position.x > area.x + area.width - halfWidth)
        player->position.x = area.x + area.width - halfWidth;

    // Top boundary
    if (player->position.y < area.y + halfHeight)
        player->position.y = area.y + halfHeight;

    // Bottom boundary
    if (player->position.y > area.y + area.height - halfHeight)
        player->position.y = area.y + area.height - halfHeight;
}

Vector2 PlayerGetThrowDirection(const Player *player)
{
    Vector2 forward;

    if (player->side == PLAYER_SIDE_LEFT)
        forward = (Vector2){1.0f, 0.0f};
    else
        forward = (Vector2){-1.0f, 0.0f};

    float angle = 0.0f;

    bool up = IsKeyDown(KEY_W);
    bool down = IsKeyDown(KEY_S);
    bool sideAngle = player->side == PLAYER_SIDE_LEFT ? IsKeyDown(KEY_A) : IsKeyDown(KEY_D);

    /*
     * A + W / D + W = maximum upward angle
     * W              = half upward angle
     * Nothing        = straight forward
     * S              = half downward angle
     * A + S / D + S = maximum downward angle
     */
    if (up)
        angle = sideAngle ? -player->throwAngle : -player->throwAngle * 0.5f;
    else if (down)
        angle = sideAngle ? player->throwAngle : player->throwAngle * 0.5f;

    float radians = angle * DEG2RAD;

    Vector2 direction;

    direction.x = forward.x * cosf(radians) - forward.y * sinf(radians);
    direction.y = forward.x * sinf(radians) + forward.y * cosf(radians);

    return direction;
}