#include "background.h"
#include "raylib.h"

// Initialize background
void BackgroundInit(Background *background)
{
    background->frame = 0;
    background->frameCount = 0;
    background->animationTimer = 0.0f;
    background->frameTime = 0.09f;
    background->objectCount = 0;

    for (int i = 0; i < MAX_BACKGROUND_FRAMES; i++)
        background->frames[i] = (Texture2D){0};
}

// Add animation frame
void BackgroundAddFrame(Background *background, const char *texturePath)
{
    if (background->frameCount >= MAX_BACKGROUND_FRAMES)
        return;

    background->frames[background->frameCount] = LoadTexture(texturePath);
    SetTextureFilter(background->frames[background->frameCount], TEXTURE_FILTER_POINT);

    background->frameCount++;
}

// Add fixed background object
void BackgroundAddObject(Background *background, const char *texturePath, Vector2 position)
{
    if (background->objectCount >= MAX_BACKGROUND_OBJECTS)
        return;

    BackgroundObject *object = &background->objects[background->objectCount];

    object->texture = LoadTexture(texturePath);
    object->position = position;

    SetTextureFilter(object->texture, TEXTURE_FILTER_POINT);

    background->objectCount++;
}

// Update background animation
void BackgroundUpdate(Background *background)
{
    if (background->frameCount <= 1)
        return;

    background->animationTimer += GetFrameTime();

    if (background->animationTimer >= background->frameTime)
    {
        background->animationTimer -= background->frameTime;
        background->frame++;

        if (background->frame >= background->frameCount)
            background->frame = 0;
    }
}

// Draw background
void BackgroundDraw(const Background *background)
{
    if (background->frameCount > 0)
    {
        DrawTexture(background->frames[background->frame], 8, 0, WHITE);
    }

    // Draw fixed objects
    for (int i = 0; i < background->objectCount; i++)
        DrawTexture(background->objects[i].texture, background->objects[i].position.x, background->objects[i].position.y, WHITE);
}

// Unload background
void BackgroundUnload(Background *background)
{
    for (int i = 0; i < background->frameCount; i++)
    {
        if (background->frames[i].id != 0)
            UnloadTexture(background->frames[i]);
    }

    for (int i = 0; i < background->objectCount; i++)
    {
        if (background->objects[i].texture.id != 0)
            UnloadTexture(background->objects[i].texture);
    }
}