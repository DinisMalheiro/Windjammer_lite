#include "game.h"
#include "raymath.h"
#include <math.h>

static void GameUpdate(Game *game);
static void GameDraw(Game *game);
static void GameUpdateGameplay(Game *game);
static void GameDrawGameplay(Game *game);
static void GameDrawMenu(Game *game);
static void GameUpdateDiskCollision(Game *game);
static int GameGetGoalZone(const Goal *goal, Vector2 diskPosition);
static void GameHandleGoal(Game *game, int side, int zone);
static bool GameTryCatchDisk(Game *game);
static void GameStartShake(Game *game, float duration, float strength);
static void GameUpdateRoundEnd(Game *game);
static void GameDrawRoundEnd(Game *game);

void GameInit(Game *game) {
  *game = (Game){0};

  game->gameWidth = 320;
  game->gameHeight = 224;
  game->screen = GAME_SCREEN_MENU;

  // Window
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(800, 600, "Windjammer_lite");
  // Initialize audio
  InitAudioDevice();
  SoundManagerInit(&game->sound);

  SetExitKey(KEY_NULL);
  SetTargetFPS(60);

  // Fixed-resolution render target
  game->target = LoadRenderTexture(game->gameWidth, game->gameHeight);
  SetTextureFilter(game->target.texture, TEXTURE_FILTER_POINT);

  // Load all textures through the central asset manager
  AssetsLoad(&game->assets);

  // Initialize the main menu
  MenuInit(&game->menu);

  // Setup animated background
  BackgroundInit(&game->background);

  for (int i = 0; i < 4; i++)
    BackgroundSetFrame(&game->background, i, &game->assets.backgroundFrames[i]);

  // Setup fixed map objects
  BackgroundSetObject(&game->background, 0, &game->assets.mapNet, (Vector2){8, 0}, BACKGROUND_LAYER_BEHIND);
  BackgroundSetObject(&game->background, 1, &game->assets.Backgroundassets1, (Vector2){8, 0}, BACKGROUND_LAYER_BEHIND);
  BackgroundSetObject(&game->background, 2, &game->assets.Backgroundassets2, (Vector2){8, 0}, BACKGROUND_LAYER_FRONT);

  // Player and CPU movement areas
  game->playerArea = (Rectangle){0, 30, game->gameWidth / 2 + 10, game->gameHeight - 48};
  game->cpuArea = (Rectangle){game->gameWidth / 2 - 10, 30, game->gameWidth / 2 + 10, game->gameHeight - 48};

  // Player
  PlayerInit(&game->player, (Vector2){50, 120});
  PlayerSetTexture(&game->player, &game->assets.girl1Movement);

  // CPU
  CPUInit(&game->cpu, (Vector2){270, 120}, game->cpuArea);
  CPUSetTexture(&game->cpu, &game->assets.girl1Movement);

  // Disk
  DiskInit(&game->disk, game->player.position);

  game->player.hasDisk = true;
  game->cpu.hasDisk = false;

  game->disk.held = true;
  game->disk.thrown = false;
  game->disk.velocity = (Vector2){0};

  // Disk collision arena
  game->diskArena = (Rectangle){0, 40, 320, 146};

  // Goal positions
  game->leftGoal.x = 0;
  game->rightGoal.x = 320;

  // Goal zones
  game->leftGoal.top = 30;
  game->leftGoal.zone1Bottom = 70;
  game->leftGoal.zone2Bottom = 136;
  game->leftGoal.bottom = 176;

  game->rightGoal.top = 30;
  game->rightGoal.zone1Bottom = 70;
  game->rightGoal.zone2Bottom = 136;
  game->rightGoal.bottom = 176;

  // Score
  game->leftScore = 0;
  game->rightScore = 0;

  // Game timer
  game->gameTime = 100.0f;
  game->gameTimer = game->gameTime;

  // Menu music
  SoundManagerLoadMusic(&game->sound, "assets/Sounds/Start_song.wav");
  // Menu sounds
  game->soundMenuMove = SoundManagerLoadSound(&game->sound, "assets/Sounds/menu1.wav");
  game->soundMenuSelect = SoundManagerLoadSound(&game->sound, "assets/Sounds/menu2.wav");
  // Gameplay sounds
  game->soundThrow = SoundManagerLoadSound(&game->sound, "assets/Sounds/Diskthrow2.wav");
  game->sound3Points = SoundManagerLoadSound(&game->sound, "assets/Sounds/3points.wav");
  game->sound5Points = SoundManagerLoadSound(&game->sound, "assets/Sounds/5points.wav");
  game->soundDiskHitwall = SoundManagerLoadSound(&game->sound, "assets/Sounds/diskHitWall.wav");
  SoundManagerPlayMusic(&game->sound);
}

void GameRun(Game *game) {
  while (!WindowShouldClose()) {
    GameUpdate(game);
    GameDraw(game);
  }
}

static void GameUpdate(Game *game) {
  SoundManagerUpdate(&game->sound);

  if (game->shakeTimer > 0.0f) {
    game->shakeTimer -= GetFrameTime();

    if (game->shakeTimer < 0.0f)
      game->shakeTimer = 0.0f;
  }

  // do movesound on menu
  if (game->screen == GAME_SCREEN_MENU || game->screen == GAME_SCREEN_PAUSE) {
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_S)) {
      SoundManagerPlaySound(&game->sound, game->soundMenuMove);
    }
  }

  switch (game->screen) {
  case GAME_SCREEN_MENU: {
    int startGame = 0;
    int exitGame = 0;

    MenuUpdateMain(&game->menu, &startGame, &exitGame);

    if (startGame) {
      SoundManagerPlaySound(&game->sound, game->soundMenuSelect);
      // Start a completely new match.
      GameReset(game);
      // Change from menu music to gameplay music.
      SoundManagerChangeMusic(&game->sound, "assets/sounds/wjammers-Beach_song.wav");

      game->screen = GAME_SCREEN_GAMEPLAY;
    }

    if (exitGame) {
      SoundManagerPlaySound(&game->sound, game->soundMenuSelect);
      CloseWindow();
    }

    break;
  }

  case GAME_SCREEN_GAMEPLAY:

    // ESC opens pause menu
    if (IsKeyPressed(KEY_ESCAPE)) {
      game->screen = GAME_SCREEN_PAUSE;
      break;
    }

    // Only update gameplay when NOT paused
    GameUpdateGameplay(game);

    break;

  case GAME_SCREEN_PAUSE: {
    int continueGame = 0;
    int exitToMenu = 0;

    MenuUpdatePause(&game->menu, &continueGame, &exitToMenu);

    if (continueGame) {
      SoundManagerPlaySound(&game->sound, game->soundMenuSelect);
      game->screen = GAME_SCREEN_GAMEPLAY;
    }

    if (exitToMenu) {
      SoundManagerPlaySound(&game->sound, game->soundMenuSelect);
      game->screen = GAME_SCREEN_MENU;

      // Stop gameplay music and start menu music.
      SoundManagerChangeMusic(&game->sound, "assets/Sounds/Start_song.wav");
    }

    break;
  }

  case GAME_SCREEN_ROUND_END:
    GameUpdateRoundEnd(game);
    break;
  }
}

static void GameUpdateGameplay(Game *game) {
  float deltaTime = GetFrameTime();

  // Update background animation
  BackgroundUpdate(&game->background);

  // Update game timer
  if (game->gameTimer > 0.0f) {
    game->gameTimer -= deltaTime;

    if (game->gameTimer <= 0.0f) {
      game->gameTimer = 0.0f;

      TraceLog(LOG_INFO, "ROUND END - PLAYER: %d CPU: %d", game->leftScore, game->rightScore);

      game->screen = GAME_SCREEN_ROUND_END;
      return;
    }
  }

  // Update player
  PlayerUpdate(&game->player);
  PlayerUpdateMapLimits(&game->player, game->playerArea);

  // Update CPU
  CPUUpdate(&game->cpu, &game->player, &game->disk);

  // Player throws disk
  if (game->player.hasDisk && IsKeyPressed(KEY_SPACE)) {
    Vector2 throwDirection = PlayerGetThrowDirection(&game->player);

    SoundManagerPlaySound(&game->sound, game->soundThrow);

    game->player.hasDisk = false;
    game->player.canMove = false;
    game->player.throwTimer = game->player.throwDelay;

    game->disk.position = game->player.position;

    DiskThrow(&game->disk, throwDirection);
  }

  if (game->cpu.hasDisk && !game->disk.thrown) {
    SoundManagerPlaySound(&game->sound, game->soundThrow);
  }

  // Update disk movement
  DiskUpdate(&game->disk);

  // Check disk walls and goals
  GameUpdateDiskCollision(game);

  // Check player catching disk
  GameTryCatchDisk(game);
}

static bool GameTryCatchDisk(Game *game) {
  Disk *disk = &game->disk;
  Player *player = &game->player;

  // Disk must be thrown and catch delay must be finished
  if (!disk->thrown || disk->catchTimer > 0.0f)
    return false;

  // Check distance between player and disk
  if (!DiskPlayerCollision(disk, player->position, player->catchRadius))
    return false;

  // Pick up disk
  DiskPickup(disk);

  player->hasDisk = true;
  player->canMove = false;

  return true;
}

static void GameUpdateDiskCollision(Game *game) {
  Disk *disk = &game->disk;
  Rectangle arena = game->diskArena;

  if (!disk->thrown)
    return;

  const float radius = 6.0f;

  // Top wall collision
  if (disk->position.y - radius <= arena.y) {
    disk->position.y = arena.y + radius;
    disk->velocity.y = fabsf(disk->velocity.y);
    SoundManagerPlaySound(&game->sound, game->soundDiskHitwall);
  }

  // Bottom wall collision
  if (disk->position.y + radius >= arena.y + arena.height) {
    disk->position.y = arena.y + arena.height - radius;
    disk->velocity.y = -fabsf(disk->velocity.y);
    SoundManagerPlaySound(&game->sound, game->soundDiskHitwall);
  }

  // Left goal
  if (disk->position.x <= game->leftGoal.x) {
    int zone = GameGetGoalZone(&game->leftGoal, disk->position);

    if (zone != 0) {
      GameHandleGoal(game, -1, zone);
      return;
    }
  }

  // Right goal
  if (disk->position.x >= game->rightGoal.x) {
    int zone = GameGetGoalZone(&game->rightGoal, disk->position);

    if (zone != 0) {
      GameHandleGoal(game, 1, zone);
      return;
    }
  }
}

static int GameGetGoalZone(const Goal *goal, Vector2 diskPosition) {
  if (diskPosition.y < goal->top)
    return 0;

  if (diskPosition.y < goal->zone1Bottom)
    return 1;

  if (diskPosition.y < goal->zone2Bottom)
    return 2;

  if (diskPosition.y <= goal->bottom)
    return 3;

  return 0;
}

static void GameHandleGoal(Game *game, int side, int zone) {
  if (side < 0) {
    GameStartShake(game, 0.35f, 5.0f);
    // Player receives the disk after a goal
    game->player.hasDisk = true;
    game->cpu.hasDisk = false;

    game->disk.held = true;
    game->disk.thrown = false;
    game->disk.velocity = (Vector2){0};
    game->disk.position = game->player.position;
    if (zone == 1) {
      game->rightScore += 3;
      SoundManagerPlaySound(&game->sound, game->sound3Points);
    } else if (zone == 2) {
      game->rightScore += 5;
      SoundManagerPlaySound(&game->sound, game->sound5Points);
    } else if (zone == 3) {
      game->rightScore += 3;
      SoundManagerPlaySound(&game->sound, game->sound3Points);
    }

    TraceLog(LOG_INFO, "LEFT GOAL - ZONE %d - RIGHT SCORE: %d", zone, game->rightScore);
  } else {
    GameStartShake(game, 0.35f, 5.0f);
    // CPU receives the disk after a goal
    game->player.hasDisk = false;
    game->cpu.hasDisk = true;

    game->disk.held = true;
    game->disk.thrown = false;
    game->disk.velocity = (Vector2){0};
    game->disk.position = game->cpu.position;

    game->cpu.throwTimer = game->cpu.throwDelay;
    if (zone == 1) {
      game->leftScore += 3;
      SoundManagerPlaySound(&game->sound, game->sound3Points);
    } else if (zone == 2) {
      game->leftScore += 5;
      SoundManagerPlaySound(&game->sound, game->sound5Points);
    } else if (zone == 3) {
      game->leftScore += 3;
      SoundManagerPlaySound(&game->sound, game->sound3Points);
    }
    TraceLog(LOG_INFO, "RIGHT GOAL - ZONE %d - LEFT SCORE: %d", zone, game->leftScore);
  }
}

static void GameDraw(Game *game) {
  // Draw everything to the fixed-resolution render target
  BeginTextureMode(game->target);

  ClearBackground(BLACK);

  switch (game->screen) {
  case GAME_SCREEN_MENU:
    GameDrawMenu(game);
    break;

  case GAME_SCREEN_GAMEPLAY:
    GameDrawGameplay(game);
    break;

  case GAME_SCREEN_PAUSE:
    // Draw the game underneath the pause menu
    GameDrawGameplay(game);
    MenuDrawPause(&game->menu);
    break;

  case GAME_SCREEN_ROUND_END:
    GameDrawRoundEnd(game);
    break;
  }

  EndTextureMode();

  // Scale the fixed-resolution game to the window
  BeginDrawing();

  ClearBackground(BLACK);

  float scaleX = (float)GetScreenWidth() / game->gameWidth;
  float scaleY = (float)GetScreenHeight() / game->gameHeight;
  float scale = scaleX;

  if (scaleY < scale)
    scale = scaleY;

  float width = game->gameWidth * scale;
  float height = game->gameHeight * scale;

  Rectangle source = {0, 0, (float)game->target.texture.width, -(float)game->target.texture.height};

  float shakeX = 0.0f;
  float shakeY = 0.0f;

  if (game->shakeTimer > 0.0f) {
    shakeX = (float)GetRandomValue(-(int)game->shakeStrength, (int)game->shakeStrength);

    shakeY = (float)GetRandomValue(-(int)game->shakeStrength, (int)game->shakeStrength);
  }

  Rectangle dest = {(GetScreenWidth() - width) / 2 + shakeX, (GetScreenHeight() - height) / 2 + shakeY, width, height};

  DrawTexturePro(game->target.texture, source, dest, (Vector2){0, 0}, 0.0f, WHITE);

  EndDrawing();
}

static void GameDrawMenu(Game *game) { MenuDrawMain(&game->menu); }

static void GameDrawGameplay(Game *game) {
  BackgroundDrawBehind(&game->background);
  DiskDraw(&game->disk);
  PlayerDraw(&game->player);
  CPUDraw(&game->cpu);
  BackgroundDrawFront(&game->background);
}

void GameUnload(Game *game) {
  DiskUnload(&game->disk);
  PlayerUnload(&game->player);
  CPUUnload(&game->cpu);
  BackgroundUnload(&game->background);

  AssetsUnload(&game->assets);

  UnloadRenderTexture(game->target);

  SoundManagerUnload(&game->sound);
  CloseAudioDevice();

  CloseWindow();
}

void GameReset(Game *game) {
  // Reset players
  PlayerInit(&game->player, (Vector2){50, 120});
  PlayerSetTexture(&game->player, &game->assets.girl1Movement);

  CPUInit(&game->cpu, (Vector2){270, 120}, game->cpuArea);
  CPUSetTexture(&game->cpu, &game->assets.girl1Movement);

  // Player starts with the disk
  game->player.hasDisk = true;
  game->player.canMove = false;

  game->cpu.hasDisk = false;

  // Reset disk
  DiskInit(&game->disk, game->player.position);

  game->disk.held = true;
  game->disk.thrown = false;
  game->disk.velocity = (Vector2){0};
  game->disk.position = game->player.position;

  // Reset score
  game->leftScore = 0;
  game->rightScore = 0;

  // Reset timer
  game->gameTimer = game->gameTime;

  // Reset background animation
  game->background.frame = 0;
  game->background.animationTimer = 0.0f;
}

static void GameStartShake(Game *game, float duration, float strength) {
  game->shakeTimer = duration;
  game->shakeDuration = duration;
  game->shakeStrength = strength;
}

static void GameUpdateRoundEnd(Game *game) {
  if (IsKeyPressed(KEY_ENTER)) {
    // Restart match
    GameReset(game);

    SoundManagerChangeMusic(&game->sound, "assets/sounds/wjammers-Beach_song.wav");

    game->screen = GAME_SCREEN_GAMEPLAY;
  }

  if (IsKeyPressed(KEY_ESCAPE)) {
    // Return to main menu
    SoundManagerChangeMusic(&game->sound, "assets/Sounds/Start_song.wav");

    game->screen = GAME_SCREEN_MENU;
  }
}

static void GameDrawRoundEnd(Game *game) {
  DrawText("TIME UP!", 115, 45, 24, WHITE);

  DrawText(TextFormat("PLAYER: %d", game->leftScore), 90, 90, 16, WHITE);

  DrawText(TextFormat("CPU: %d", game->rightScore), 180, 90, 16, WHITE);

  if (game->leftScore > game->rightScore) {
    DrawText("YOU WIN!", 115, 125, 18, WHITE);
  } else if (game->leftScore < game->rightScore) {
    DrawText("CPU WINS!", 105, 125, 18, WHITE);
  } else {
    DrawText("DRAW!", 130, 125, 18, WHITE);
  }

  DrawText("ENTER - RESTART", 85, 165, 12, WHITE);

  DrawText("ESC - MENU", 105, 185, 12, WHITE);
}