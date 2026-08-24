#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"

typedef enum PlayerAnimation
{
    PLAYER_ANIM_WALK_RIGHT,
    PLAYER_ANIM_WALK_LEFT,
    PLAYER_ANIM_WALK_DOWN,
    PLAYER_ANIM_WALK_UP,
    PLAYER_ANIM_IDLE
} PlayerAnimation;

typedef enum PlayerSide
{
    PLAYER_SIDE_LEFT,
    PLAYER_SIDE_RIGHT
} PlayerSide;

typedef struct Player
{
    Vector2 position;
    Vector2 velocity;
    Vector2 facingDirection;

    bool canMove;
    float speed;

    bool hasDisk;
    PlayerSide side;

    // Throwing
    float throwAngle;

    // Disk catching
    float catchRadius;
    float throwTimer;
    float throwDelay;

    // Shared sprite asset
    Texture2D *sprite;

    // Sprite sheet
    int frame;
    int currentRow;
    int columns;
    int rows;
    int frameWidth;
    int frameHeight;

    // Animation
    float animationTimer;
    float frameTime;
    PlayerAnimation animation;
} Player;

void PlayerInit(Player *player, Vector2 position);
void PlayerUpdate(Player *player);
void PlayerDraw(const Player *player);
void PlayerUnload(Player *player);
void PlayerSetTexture(Player *player, Texture2D *texture);
void PlayerUpdateMapLimits(Player *player, Rectangle area);

Vector2 PlayerGetThrowDirection(const Player *player);

#endif