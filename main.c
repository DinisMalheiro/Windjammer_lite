#include "raylib.h"
#include "stdio.h"
#include "game.h"

//how to run
// gcc main.c player.c background.c -g -o main.exe -lraylib -lopengl32 -lgdi32 -lwinmm
// gcc main.c game.c player.c background.c disk.c -g -o main.exe -lraylib -lopengl32 -lgdi32 -lwinmm
// ./main.exe


int main(void)
{
    Game game;

    GameInit(&game);
    GameRun(&game);
    GameUnload(&game);

    return 0;
}