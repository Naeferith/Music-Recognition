#include "DebugAudioRecorder.h"
#include <iostream>

#include "ModelFFT.h"

sf::SoundBuffer DebugAudioRecorder::quickbuffer = sf::SoundBuffer();

bool DebugAudioRecorder::onStart() {
	bool ret = SoundBufferRecorder::onStart();
	std::cout << "Start recording..." << std::endl;
	return ret;
}

void DebugAudioRecorder::onStop() {
	SoundBufferRecorder::onStop();
	std::cout << "Stoped recording..." << std::endl;
}

bool DebugAudioRecorder::onProcessSamples(const sf::Int16 * samples, std::size_t sampleCount)
{
	bool ret = SoundBufferRecorder::onProcessSamples(samples, sampleCount);
	std::cout << sampleCount << " samples." << std::endl;

	//RealTime
	/*if (quickbuffer.loadFromSamples(samples, sampleCount, 1, 11025)) {
		ModelFFT myFFT(quickbuffer, POW2_10);
	}*/

	return ret;
}
