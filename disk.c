#include "disk.h"
#include <math.h>

void DiskInit(Disk *disk, Vector2 position)
{
    disk->position = position;
    disk->velocity = (Vector2){0};
    disk->speed = 350.0f;
    disk->catchTimer = 0.0f;
    disk->held = false;
    disk->thrown = false;
    disk->texture = (Texture2D){0};
}

void DiskUpdate(Disk *disk)
{
    if (!disk->thrown)
        return;

    float deltaTime = GetFrameTime();

    if (disk->catchTimer > 0.0f) // Time remaining before the disk can be caught again
    {
        disk->catchTimer -= deltaTime;

        if (disk->catchTimer < 0.0f)
            disk->catchTimer = 0.0f;
    }

    disk->position.x += disk->velocity.x * deltaTime;
    disk->position.y += disk->velocity.y * deltaTime;
}

void DiskDraw(const Disk *disk)
{
    if (disk->held)
        return;

    DrawCircleV(disk->position, 6.0f, YELLOW);
}

void DiskUnload(Disk *disk)
{
    if (disk->texture.id != 0)
        UnloadTexture(disk->texture);

    disk->texture = (Texture2D){0};
}

bool DiskPlayerCollision(const Disk *disk, Vector2 playerPosition, float radius)
{
    float dx = disk->position.x - playerPosition.x;
    float dy = disk->position.y - playerPosition.y;

    return sqrtf(dx * dx + dy * dy) <= radius;
}

void DiskPickup(Disk *disk)
{
    disk->held = true;
    disk->thrown = false;
    disk->velocity = (Vector2){0};
    disk->catchTimer = 0.0f;
}

void DiskThrow(Disk *disk, Vector2 direction)
{
    float length = sqrtf(direction.x * direction.x + direction.y * direction.y);

    if (length <= 0.0f)
        return;

    direction.x /= length;
    direction.y /= length;

    disk->held = false;
    disk->thrown = true;
    disk->catchTimer = 0.25f;
    disk->velocity.x = direction.x * disk->speed;
    disk->velocity.y = direction.y * disk->speed;
}