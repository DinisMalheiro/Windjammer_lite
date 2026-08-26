#include "menu.h"

// Initialize menu
void MenuInit(Menu *menu) {
  *menu = (Menu){0};

  menu->selectedOption = 0;
  menu->mainOption = MENU_OPTION_PLAY;
  menu->pauseOption = PAUSE_OPTION_CONTINUE;

  menu->blinkTimer = 0.0f;
}

// Update main menu
void MenuUpdateMain(Menu *menu, int *startGame, int *exitGame) {
  float deltaTime = GetFrameTime();

  menu->blinkTimer += deltaTime;

  // Move selection
  if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
    if (menu->mainOption == MENU_OPTION_EXIT)
      menu->mainOption = MENU_OPTION_PLAY;
  }

  if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
    if (menu->mainOption == MENU_OPTION_PLAY)
      menu->mainOption = MENU_OPTION_EXIT;
  }

  // Select option
  if (IsKeyPressed(KEY_ENTER)) {
    if (menu->mainOption == MENU_OPTION_PLAY)
      *startGame = 1;
    else
      *exitGame = 1;
  }
}

// Update pause menu
void MenuUpdatePause(Menu *menu, int *continueGame, int *exitToMenu) {
  float deltaTime = GetFrameTime();

  menu->blinkTimer += deltaTime;

  // ESC immediately continues the game
  if (IsKeyPressed(KEY_ESCAPE)) {
    *continueGame = 1;
    return;
  }

  // Move selection
  if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
    if (menu->pauseOption == PAUSE_OPTION_EXIT)
      menu->pauseOption = PAUSE_OPTION_CONTINUE;
  }

  if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
    if (menu->pauseOption == PAUSE_OPTION_CONTINUE)
      menu->pauseOption = PAUSE_OPTION_EXIT;
  }

  // Select option
  if (IsKeyPressed(KEY_ENTER)) {
    if (menu->pauseOption == PAUSE_OPTION_CONTINUE)
      *continueGame = 1;
    else
      *exitToMenu = 1;
  }
}

// Draw main menu
void MenuDrawMain(const Menu *menu) {
  DrawText("WINDJAMMER LITE", 60, 55, 24, WHITE);

  // PLAY
  if (menu->mainOption == MENU_OPTION_PLAY)
    DrawText(">", 80, 115, 20, YELLOW);

  DrawText("PLAY", 105, 115, 20, WHITE);

  // EXIT
  if (menu->mainOption == MENU_OPTION_EXIT)
    DrawText(">", 80, 150, 20, YELLOW);

  DrawText("EXIT", 105, 150, 20, WHITE);
}

// Draw pause menu
void MenuDrawPause(const Menu *menu) {
  // Dark overlay
  DrawRectangle(0, 0, 320, 224, Fade(BLACK, 0.65f));

  DrawText("PAUSED", 115, 55, 24, WHITE);

  // CONTINUE
  if (menu->pauseOption == PAUSE_OPTION_CONTINUE)
    DrawText(">", 80, 115, 20, YELLOW);

  DrawText("CONTINUE", 105, 115, 20, WHITE);

  // EXIT
  if (menu->pauseOption == PAUSE_OPTION_EXIT)
    DrawText(">", 80, 150, 20, YELLOW);

  DrawText("EXIT TO MENU", 105, 150, 20, WHITE);
}