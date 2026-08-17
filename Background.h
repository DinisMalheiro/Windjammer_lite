#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "raylib.h"

#define MAX_BACKGROUND_FRAMES 8
#define MAX_BACKGROUND_OBJECTS 32

typedef struct BackgroundObject
{
    Texture2D texture;
    Vector2 position;
} BackgroundObject;

typedef struct Background
{
    Texture2D frames[MAX_BACKGROUND_FRAMES];

    int frame;
    int frameCount;

    float animationTimer;
    float frameTime;

    BackgroundObject objects[MAX_BACKGROUND_OBJECTS];
    int objectCount;
} Background;

void BackgroundInit(Background *background);
void BackgroundUpdate(Background *background);
void BackgroundDraw(const Background *background);
void BackgroundUnload(Background *background);

void BackgroundAddFrame(Background *background, const char *texturePath);
void BackgroundAddObject(Background *background, const char *texturePath, Vector2 position);

#endif