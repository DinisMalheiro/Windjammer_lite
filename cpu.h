#ifndef CPU_H
#define CPU_H

#include "raylib.h"
#include "disk.h"
#include "player.h"

typedef struct CPU
{
    Vector2 position;
    Vector2 velocity;

    bool returning;
    float speed;
    float catchRadius;
    float throwAngle;

    bool hasDisk;

    float throwTimer;
    float throwDelay;

    float moveTimer;
    float moveDelay;

    float homeY;

    Rectangle area;

    PlayerSide side;

    Texture2D sprite;

    int frame;
    int currentRow;
    int columns;
    int rows;
    int frameWidth;
    int frameHeight;

    float animationTimer;
    float frameTime;

} CPU;

void CPUInit(CPU *cpu, Vector2 position, Rectangle area);
void CPUUpdate(CPU *cpu, const Player *player, Disk *disk);
void CPUDraw(const CPU *cpu);
void CPUSetTexture(CPU *cpu, const char *texturePath);
void CPUUnload(CPU *cpu);

#endif