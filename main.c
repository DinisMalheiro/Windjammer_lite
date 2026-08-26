#include "raylib.h"
#include "stdio.h"
#include "game.h"

//author: Dinis Malheiro

//how to run
// gcc main.c game.c player.c cpu.c background.c disk.c assets.c menu.c sound.c -g -o main.exe -lraylib -lopengl32 -lgdi32 -lwinmm
// ./main.exe

//Required libraries: raylib
//for Windows use ucrt64


int main(void)
{
    Game game;

    GameInit(&game);
    GameRun(&game);
    GameUnload(&game);

    return 0;
}