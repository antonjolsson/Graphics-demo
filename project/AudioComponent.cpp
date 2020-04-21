#include "AudioComponent.h"

void AudioComponent::playMusic(const Audio _music)
{
	if (!_music.isMusic) return;
    Mix_VolumeMusic(_music.volume);
    currentMusic = Mix_LoadMUS(_music.AUDIO_FILE);
    if (!currentMusic) {
        printf("Mix_LoadMUS(\"%s\"): %s\n", _music.AUDIO_FILE, Mix_GetError());
    }
    else Mix_PlayMusic(currentMusic, -1);
}

void AudioComponent::playSound(const Audio _soundEffect)
{
	if (_soundEffect.isMusic) return;
    currentSound = Mix_LoadWAV(_soundEffect.AUDIO_FILE);
    Mix_VolumeChunk(currentSound, _soundEffect.volume);
    Mix_PlayChannel(-1, currentSound, 0);
}

void AudioComponent::destroy()
{
    delete currentSound;
    delete currentMusic;
}
