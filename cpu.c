#include "cpu.h"
#include "raymath.h"
#include <math.h>

static void CPUSetAnimation(CPU *cpu, int row)
{
    if (cpu->currentRow == row)
        return;

    cpu->currentRow = row;
    cpu->frame = 0;
    cpu->animationTimer = 0.0f;
}

void CPUInit(CPU *cpu, Vector2 position, Rectangle area)
{
    cpu->position = position;
    cpu->velocity = (Vector2){0};
    cpu->returning = false;
    cpu->throwAngle = 60.0f;

    cpu->speed = 150.0f;
    cpu->catchRadius = 20.0f;

    cpu->hasDisk = false;

    cpu->throwTimer = 0.0f;
    cpu->throwDelay = 1.0f;

    cpu->moveTimer = 0.0f;
    cpu->moveDelay = 0.5f; 

    cpu->homeY = position.y;
    cpu->area = area;

    cpu->side = PLAYER_SIDE_RIGHT;

    cpu->sprite = (Texture2D){0};

    cpu->frame = 0;
    cpu->currentRow = 4;
    cpu->columns = 5;
    cpu->rows = 6;
    cpu->frameWidth = 64;
    cpu->frameHeight = 64;

    cpu->animationTimer = 0.0f;
    cpu->frameTime = 0.12f;
}

void CPUUpdate(CPU *cpu, const Player *player, Disk *disk)
{
    float deltaTime = GetFrameTime();

    cpu->velocity = (Vector2){0};

    // CPU has disk: cannot move, wait then throw
    if (cpu->hasDisk)
    {
        CPUSetAnimation(cpu, 4);

        if (cpu->throwTimer > 0.0f)
        {
            cpu->throwTimer -= deltaTime;
            goto animation;
        }

        int throwType = GetRandomValue(0, 4);
        float angle = 0.0f;

        if (throwType == 0)
            angle = -cpu->throwAngle;
        else if (throwType == 1)
            angle = -cpu->throwAngle * 0.5f;
        else if (throwType == 2)
            angle = 0.0f;
        else if (throwType == 3)
            angle = cpu->throwAngle * 0.5f;
        else if (throwType == 4)
            angle = cpu->throwAngle;

        float radians = angle * DEG2RAD;

        Vector2 forward = {-1.0f, 0.0f};
        Vector2 direction;

        direction.x = forward.x * cosf(radians) - forward.y * sinf(radians);
        direction.y = forward.x * sinf(radians) + forward.y * cosf(radians);

        disk->position = cpu->position;
        DiskThrow(disk, direction);

        cpu->hasDisk = false;
        cpu->moveTimer = cpu->moveDelay;
        cpu->returning = true;

        goto animation;
    }

    /*
    // Delay before returning to center
    if (cpu->returning && cpu->moveTimer > 0.0f)
    {
        cpu->moveTimer -= deltaTime;
        goto animation;
    }

    // Return to center
    if (cpu->returning)
    {
        float difference = cpu->homeY - cpu->position.y;

        if (fabsf(difference) > 2.0f)
        {
            cpu->velocity.y = difference > 0.0f ? 1.0f : -1.0f;
            cpu->position.y += cpu->velocity.y * cpu->speed * deltaTime;
            goto animation;
        }

        cpu->position.y = cpu->homeY;
        cpu->returning = false;
        goto animation;
    }
    */

    // Only defend when the disk enters CPU's area
    if (disk->thrown && CheckCollisionPointRec(disk->position, cpu->area))
    {
        float difference = disk->position.y - cpu->position.y;

        if (fabsf(difference) > 2.0f)
        {
            cpu->velocity.y = difference > 0.0f ? 1.0f : -1.0f;
            cpu->position.y += cpu->velocity.y * cpu->speed * deltaTime;
        }

        if (disk->catchTimer <= 0.0f && Vector2Distance(cpu->position, disk->position) <= cpu->catchRadius)
        {
            disk->position = cpu->position;
            disk->velocity = (Vector2){0};
            disk->thrown = false;
            disk->held = true;

            cpu->hasDisk = true;
            cpu->throwTimer = cpu->throwDelay;
        }
    }

    // Keep CPU inside its area
    if (cpu->position.y < cpu->area.y)
        cpu->position.y = cpu->area.y;

    if (cpu->position.y > cpu->area.y + cpu->area.height)
        cpu->position.y = cpu->area.y + cpu->area.height;

animation:

    if (cpu->velocity.y < 0.0f)
        CPUSetAnimation(cpu, 3);
    else if (cpu->velocity.y > 0.0f)
        CPUSetAnimation(cpu, 2);
    else
        CPUSetAnimation(cpu, 4);

    cpu->animationTimer += deltaTime;

    if (cpu->animationTimer >= cpu->frameTime)
    {
        cpu->animationTimer -= cpu->frameTime;
        cpu->frame++;

        if (cpu->frame >= cpu->columns)
            cpu->frame = 0;
    }
}

void CPUDraw(const CPU *cpu)
{
    if (cpu->sprite.id != 0)
    {
        Rectangle sourceRec = {
            (float)(cpu->frame * cpu->frameWidth),
            (float)(cpu->currentRow * cpu->frameHeight),
            -(float)cpu->frameWidth,
            (float)cpu->frameHeight
        };

        Rectangle destRec = {
            cpu->position.x,
            cpu->position.y,
            (float)cpu->frameWidth,
            (float)cpu->frameHeight
        };

        Vector2 origin = {
            (float)cpu->frameWidth / 2.0f,
            (float)cpu->frameHeight / 2.0f
        };

        DrawTexturePro(cpu->sprite, sourceRec, destRec, origin, 0.0f, WHITE);
    }
    else
    {
        DrawCircleV(cpu->position, 20.0f, RED);
    }
}

void CPUSetTexture(CPU *cpu, const char *texturePath)
{
    if (cpu->sprite.id != 0)
        UnloadTexture(cpu->sprite);

    cpu->sprite = LoadTexture(texturePath);
}

void CPUUnload(CPU *cpu)
{
    if (cpu->sprite.id != 0)
    {
        UnloadTexture(cpu->sprite);
        cpu->sprite = (Texture2D){0};
    }
}