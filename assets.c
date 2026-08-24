#include "assets.h"

void AssetsLoad(GameAssets *assets)
{
    // Clear asset structure
    *assets = (GameAssets){0};

    // Player/CPU
    assets->girl1Movement = LoadTexture("assets/Girl1_movement.png");
    SetTextureFilter(assets->girl1Movement, TEXTURE_FILTER_POINT);

    // Background animation
    assets->backgroundFrames[0] = LoadTexture("assets/Sandmap/wjammers001.png");
    assets->backgroundFrames[1] = LoadTexture("assets/Sandmap/wjammers002.png");
    assets->backgroundFrames[2] = LoadTexture("assets/Sandmap/wjammers003.png");
    assets->backgroundFrames[3] = LoadTexture("assets/Sandmap/wjammers004.png");

    for (int i = 0; i < 4; i++)
        SetTextureFilter(assets->backgroundFrames[i], TEXTURE_FILTER_POINT);

    // Map objects
    assets->mapNet = LoadTexture("assets/Sandmap/Map_net.png");
    SetTextureFilter(assets->mapNet, TEXTURE_FILTER_POINT);
    assets->Backgroundassets1 = LoadTexture("assets/Sandmap/Backgroundassets1.png");
    SetTextureFilter(assets->Backgroundassets1, TEXTURE_FILTER_POINT);
    assets->Backgroundassets2 = LoadTexture("assets/Sandmap/Backgroundassets2.png");
    SetTextureFilter(assets->Backgroundassets2, TEXTURE_FILTER_POINT);
}

void AssetsUnload(GameAssets *assets)
{
    // Player/CPU
    if (assets->girl1Movement.id != 0)
        UnloadTexture(assets->girl1Movement);

    // Background animation
    for (int i = 0; i < 4; i++)
    {
        if (assets->backgroundFrames[i].id != 0)
            UnloadTexture(assets->backgroundFrames[i]);
    }

    // Map objects
    if (assets->mapNet.id != 0)
        UnloadTexture(assets->mapNet);
    
    if (assets->Backgroundassets1.id != 0)
        UnloadTexture(assets->Backgroundassets1);

    if (assets->Backgroundassets2.id != 0)
        UnloadTexture(assets->Backgroundassets2);

    *assets = (GameAssets){0};
}