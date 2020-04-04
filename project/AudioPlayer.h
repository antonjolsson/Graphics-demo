// ReSharper disable CppInconsistentNaming
#pragma once

#include "component.h"

class AudioPlayer : public Component
{

protected:

	struct Audio
	{
		const char* AUDIO_FILE{};
		Uint8 volume{};
		bool isMusic{};
	};
	
	void update(float _dt) override {}
	void playMusic(Audio _music);
	void playSound(Audio _soundEffect);

	Mix_Music* currentMusic = nullptr;
	Mix_Chunk* currentSound = nullptr;

public:

	void destroy() override;

};