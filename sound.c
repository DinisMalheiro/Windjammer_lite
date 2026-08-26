#include "sound.h"

// Initialize the sound manager.
void SoundManagerInit(SoundManager *manager) {
  *manager = (SoundManager){0};

  manager->musicVolume = 1.0f;
  manager->soundVolume = 1.0f;
}

// Load background music.
void SoundManagerLoadMusic(SoundManager *manager, const char *path) {
  manager->music = LoadMusicStream(path);

  SetMusicVolume(manager->music, manager->musicVolume);
}

// Load a sound effect.
// Returns the index used to play it later.
int SoundManagerLoadSound(SoundManager *manager, const char *path) {
  if (manager->soundCount >= MAX_SOUNDS)
    return -1;

  int index = manager->soundCount;

  manager->effects[index] = LoadSound(path);

  SetSoundVolume(manager->effects[index], manager->soundVolume);

  manager->soundCount++;

  return index;
}

// Update streaming music.
// This must be called every frame while music is playing.
void SoundManagerUpdate(SoundManager *manager) {
  if (manager->music.stream.buffer != 0)
    UpdateMusicStream(manager->music);
}

// Play a sound effect.
void SoundManagerPlaySound(SoundManager *manager, int soundIndex) {
  if (soundIndex < 0 || soundIndex >= manager->soundCount)
    return;

  PlaySound(manager->effects[soundIndex]);
}

// Start the background music.
void SoundManagerPlayMusic(SoundManager *manager) {
  if (manager->music.stream.buffer != 0)
    PlayMusicStream(manager->music);
}

// Stop the background music.
void SoundManagerStopMusic(SoundManager *manager) {
  if (manager->music.stream.buffer != 0)
    StopMusicStream(manager->music);
}

// Change music volume.
void SoundManagerSetMusicVolume(SoundManager *manager, float volume) {
  if (volume < 0.0f)
    volume = 0.0f;

  if (volume > 1.0f)
    volume = 1.0f;

  manager->musicVolume = volume;

  if (manager->music.stream.buffer != 0)
    SetMusicVolume(manager->music, volume);
}

// Change sound effect volume.
void SoundManagerSetSoundVolume(SoundManager *manager, float volume) {
  if (volume < 0.0f)
    volume = 0.0f;

  if (volume > 1.0f)
    volume = 1.0f;

  manager->soundVolume = volume;

  for (int i = 0; i < manager->soundCount; i++) {
    SetSoundVolume(manager->effects[i], volume);
  }
}

// Unload all audio resources.
void SoundManagerUnload(SoundManager *manager) {
  // Stop music before unloading it.
  if (manager->music.stream.buffer != 0) {
    StopMusicStream(manager->music);
    UnloadMusicStream(manager->music);
  }

  // Unload all sound effects.
  for (int i = 0; i < manager->soundCount; i++) {
    if (manager->effects[i].stream.buffer != 0)
      UnloadSound(manager->effects[i]);
  }

  *manager = (SoundManager){0};
}

// Stop the current music and replace it with another music file.
void SoundManagerChangeMusic(SoundManager *manager, const char *path) {
  // Stop and unload the current music.
  if (manager->music.stream.buffer != 0) {
    StopMusicStream(manager->music);
    UnloadMusicStream(manager->music);
  }

  // Load the new music.
  manager->music = LoadMusicStream(path);

  SetMusicVolume(manager->music, manager->musicVolume);

  // Start the new music immediately.
  PlayMusicStream(manager->music);
}