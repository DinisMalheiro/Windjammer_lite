#ifndef DISK_H
#define DISK_H

#include "raylib.h"

typedef struct Disk
{
    Vector2 position;
    Vector2 velocity;

    float speed;

    bool held;
    bool thrown;

    Texture2D texture;
} Disk;

void DiskInit(Disk *disk, Vector2 position);
void DiskUpdate(Disk *disk);
void DiskDraw(const Disk *disk);
void DiskUnload(Disk *disk);

bool DiskPlayerCollision(const Disk *disk, const Vector2 playerPosition);

void DiskPickup(Disk *disk);
void DiskThrow(Disk *disk, Vector2 direction);

#endif