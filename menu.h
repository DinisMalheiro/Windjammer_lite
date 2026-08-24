#ifndef MENU_H
#define MENU_H

#include "raylib.h"

typedef enum MenuOption
{
    MENU_OPTION_PLAY,
    MENU_OPTION_EXIT
} MenuOption;

typedef enum PauseOption
{
    PAUSE_OPTION_CONTINUE,
    PAUSE_OPTION_EXIT
} PauseOption;

typedef struct Menu
{
    int selectedOption;

    // Used for the main menu
    MenuOption mainOption;

    // Used for the pause menu
    PauseOption pauseOption;

    // Small timer for menu effects
    float blinkTimer;

} Menu;

// Initialize menu
void MenuInit(Menu *menu);

// Update main menu
void MenuUpdateMain(Menu *menu, int *startGame, int *exitGame);

// Update pause menu
void MenuUpdatePause(Menu *menu, int *continueGame, int *exitToMenu);

// Draw main menu
void MenuDrawMain(const Menu *menu);

// Draw pause menu
void MenuDrawPause(const Menu *menu);

#endif