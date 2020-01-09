#pragma once

#include <SFML/Audio.hpp>

#include "DebugAudioRecorder.h"

using namespace std;

class AudioRecorder
{
	private:
		static AudioRecorder* instance;
		DebugAudioRecorder buffer;

		sf::SoundBuffer ppBuffer;
		bool ppDone;

		bool recording;

		AudioRecorder();

		


	public:
		~AudioRecorder();

		static AudioRecorder* getInstance();
		const bool isRecording();
		void toogleState();

		//Wrapper
		const sf::SoundBuffer& getRawBuffer();

		const sf::SoundBuffer& applyLpf(float lpfCuttof);

		const sf::SoundBuffer& downsample(float newSampleRate);

		void computePostProcessing();
		
};