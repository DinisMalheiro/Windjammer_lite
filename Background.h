#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "raylib.h"
#include "assets.h"

#define MAX_BACKGROUND_FRAMES 8
#define MAX_BACKGROUND_OBJECTS 32

typedef enum BackgroundLayer
{
    BACKGROUND_LAYER_BEHIND,
    BACKGROUND_LAYER_FRONT
} BackgroundLayer;

typedef struct BackgroundObject
{
    Texture2D *texture;
    Vector2 position;
    BackgroundLayer layer;
} BackgroundObject;

typedef struct Background
{
    Texture2D *frames[MAX_BACKGROUND_FRAMES];

    int frame;
    int frameCount;

    float animationTimer;
    float frameTime;

    BackgroundObject objects[MAX_BACKGROUND_OBJECTS];
    int objectCount;

} Background;

void BackgroundInit(Background *background);

void BackgroundSetFrame(
    Background *background,
    int index,
    Texture2D *texture
);

void BackgroundSetObject(
    Background *background,
    int index,
    Texture2D *texture,
    Vector2 position,
    BackgroundLayer layer
);

void BackgroundUpdate(Background *background);

void BackgroundDrawBehind(
    const Background *background
);

void BackgroundDrawFront(
    const Background *background
);

void BackgroundUnload(
    Background *background
);

#endif