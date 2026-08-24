#include "background.h"

void BackgroundInit(Background *background)
{
    *background = (Background){0};
    background->frameTime = 0.09f;

    for (int i = 0; i < MAX_BACKGROUND_FRAMES; i++)
        background->frames[i] = 0;

    for (int i = 0; i < MAX_BACKGROUND_OBJECTS; i++)
        background->objects[i].texture = 0;
}

void BackgroundSetFrame(Background *background, int index, Texture2D *texture)
{
    if (index < 0 || index >= MAX_BACKGROUND_FRAMES || texture == 0)
        return;

    background->frames[index] = texture;

    if (index >= background->frameCount)
        background->frameCount = index + 1;
}

void BackgroundSetObject(Background *background,int index,Texture2D *texture,Vector2 position,BackgroundLayer layer)
{
    if (index < 0 ||
        index >= MAX_BACKGROUND_OBJECTS ||
        texture == 0)
        return;

    background->objects[index].texture = texture;
    background->objects[index].position = position;
    background->objects[index].layer = layer;

    if (index >= background->objectCount)
        background->objectCount = index + 1;
}

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

void BackgroundDrawBehind(const Background *background)
{
    // Animated background
    if (background->frameCount > 0 &&
        background->frames[background->frame] != 0)
    {
        DrawTexture(
            *background->frames[background->frame],
            8,
            0,
            WHITE
        );
    }

    // Objects that go BEHIND players
    for (int i = 0; i < background->objectCount; i++)
    {
        const BackgroundObject *object = &background->objects[i];

        if (object->texture != 0 &&
            object->layer == BACKGROUND_LAYER_BEHIND)
        {
            DrawTexture(
                *object->texture,
                (int)object->position.x,
                (int)object->position.y,
                WHITE
            );
        }
    }
}

void BackgroundDrawFront(const Background *background)
{
    // Objects that go IN FRONT of players
    for (int i = 0; i < background->objectCount; i++)
    {
        const BackgroundObject *object = &background->objects[i];

        if (object->texture != 0 &&
            object->layer == BACKGROUND_LAYER_FRONT)
        {
            DrawTexture(
                *object->texture,
                (int)object->position.x,
                (int)object->position.y,
                WHITE
            );
        }
    }
}

void BackgroundUnload(Background *background)
{
    // Textures are owned by GameAssets.
    // Background only stores pointers to them.
    *background = (Background){0};
}