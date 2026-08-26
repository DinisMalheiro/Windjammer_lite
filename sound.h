#ifndef SOUND_H
#define SOUND_H

#include "raylib.h"

#define MAX_SOUNDS 16

typedef struct SoundManager {
  // Background music
  Music music;

  // Sound effects
  Sound effects[MAX_SOUNDS];

  int soundCount;

  // Volume
  float musicVolume;
  float soundVolume;

} SoundManager;

// Initialize the sound manager
void SoundManagerInit(SoundManager *manager);

// Load the background music
void SoundManagerLoadMusic(SoundManager *manager, const char *path);

// Load a sound effect and return its index
int SoundManagerLoadSound(SoundManager *manager, const char *path);

// Update music playback
void SoundManagerUpdate(SoundManager *manager);

// Play a sound effect
void SoundManagerPlaySound(SoundManager *manager, int soundIndex);

void SoundManagerChangeMusic(SoundManager *manager, const char *path);

// Play background music
void SoundManagerPlayMusic(SoundManager *manager);

// Stop background music
void SoundManagerStopMusic(SoundManager *manager);

// Set music volume
void SoundManagerSetMusicVolume(SoundManager *manager, float volume);

// Set sound effect volume
void SoundManagerSetSoundVolume(SoundManager *manager, float volume);

// Unload all audio
void SoundManagerUnload(SoundManager *manager);

#endif