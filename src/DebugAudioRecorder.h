#pragma once

#include <SFML/Audio.hpp>

class DebugAudioRecorder : public sf::SoundBufferRecorder
{
public:
	virtual bool onStart();
	virtual void onStop();
	virtual bool onProcessSamples(const sf::Int16 *samples, std::size_t sampleCount);
};