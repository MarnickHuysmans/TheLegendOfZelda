#pragma once
#include <vector>
#include "SoundEffect.h"
#include "SoundStream.h"

class SoundManager final
{
public:
	SoundManager() = default;
	~SoundManager();
	
	SoundManager(const SoundManager& other) = delete;
	SoundManager(SoundManager&& other) noexcept = delete;
	SoundManager& operator=(const SoundManager& other) = delete;
	SoundManager& operator=(SoundManager&& other) noexcept = delete;

	SoundEffect* CreateSoundEffect(const std::string& str);
	SoundStream* CreateSoundStream(const std::string& str);

	int GetSoundEffectVolume() const;
	int GetSoundStreamVolume() const;

	int IncSoundEffectVolume();
	int DecSoundEffectVolume();
	int IncSoundStreamVolume();
	int DecSoundStreamVolume();

	void StopSoundEffects();
	void StopSoundStream();

	void PauseSoundEffects();
	void ResumeSoundEffects();
private:
	std::vector<SoundEffect*> m_SoundEffects;
	std::vector<SoundStream*> m_SoundStreams;
};

