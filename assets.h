#ifndef ASSETS_H
#define ASSETS_H

#include "raylib.h"

typedef struct GameAssets
{
    // Player/CPU sprites
    Texture2D girl1Movement;

    // Background animation
    Texture2D backgroundFrames[4];

    // Fixed map objects
    Texture2D mapNet;
    Texture2D Backgroundassets1;
    Texture2D Backgroundassets2;
} GameAssets;

void AssetsLoad(GameAssets *assets);
void AssetsUnload(GameAssets *assets);

#endif