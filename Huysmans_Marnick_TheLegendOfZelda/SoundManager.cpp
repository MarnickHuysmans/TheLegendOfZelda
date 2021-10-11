#include "pch.h"
#include "SoundManager.h"
#include <fstream>
#include <algorithm>

SoundManager::~SoundManager()
{
	for (SoundEffect* soundEffect : m_SoundEffects)
	{
		delete soundEffect;
	}
	for (SoundStream* soundStream : m_SoundStreams)
	{
		delete soundStream;
	}
}

SoundEffect* SoundManager::CreateSoundEffect(const std::string& str)
{
	try
	{
		SoundEffect* soundEffect{ new SoundEffect{str} };
		m_SoundEffects.push_back(soundEffect);
		return soundEffect;
	}
	catch (const std::runtime_error & exception)
	{
		return nullptr;
	}
}

SoundStream* SoundManager::CreateSoundStream(const std::string& str)
{
	try
	{
		SoundStream* soundStream{ new SoundStream{str} };
		m_SoundStreams.push_back(soundStream);
		return soundStream;
	}
	catch (const std::runtime_error & exception)
	{
		return nullptr;
	}
}

int SoundManager::GetSoundEffectVolume() const
{
	if (m_SoundEffects.size() > 0)
	{
		return m_SoundEffects[0]->GetVolume();
	}
	return 0;
}

int SoundManager::GetSoundStreamVolume() const
{
	return SoundStream::GetVolume();
}

int SoundManager::IncSoundEffectVolume()
{
	int volume{ GetSoundEffectVolume() + 1 };
	std::for_each(m_SoundEffects.begin(), m_SoundEffects.end(), [volume](SoundEffect* soundEffect) {soundEffect->SetVolume(volume); });
	return GetSoundEffectVolume();
}

int SoundManager::DecSoundEffectVolume()
{
	int volume{ GetSoundEffectVolume() - 1 };
	std::for_each(m_SoundEffects.begin(), m_SoundEffects.end(), [volume](SoundEffect* soundEffect) {soundEffect->SetVolume(volume); });
	return GetSoundEffectVolume();
}

int SoundManager::IncSoundStreamVolume()
{
	int volume{ GetSoundStreamVolume() + 1 };
	SoundStream::SetVolume(volume);
	return GetSoundStreamVolume();
}

int SoundManager::DecSoundStreamVolume()
{
	int volume{ GetSoundStreamVolume() - 1 };
	SoundStream::SetVolume(volume);
	return GetSoundStreamVolume();
}

void SoundManager::StopSoundEffects()
{
	SoundEffect::StopAll();
}

void SoundManager::StopSoundStream()
{
	SoundStream::Stop();
}

void SoundManager::PauseSoundEffects()
{
	SoundEffect::PauseAll();
}

void SoundManager::ResumeSoundEffects()
{
	SoundEffect::ResumeAll();
}
