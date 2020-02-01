#include "DebugAudioRecorder.h"
#include <iostream>

#include "ModelFFT.h"

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
	//ModelFFT myFFT(samples, sampleCount, POW2_10);

	return ret;
}
