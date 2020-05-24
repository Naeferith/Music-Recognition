#pragma once

#include <SFML/Audio.hpp>

class DebugAudioRecorder : public sf::SoundBufferRecorder
{
private:
	static sf::SoundBuffer quickbuffer;
public:
	virtual bool onStart();
	virtual void onStop();
	virtual bool onProcessSamples(const sf::Int16 *samples, std::size_t sampleCount);
};