#include "AudioRecorder.h"
#include "Dsp.h"

#include <iostream>

AudioRecorder* AudioRecorder::instance = nullptr;

AudioRecorder::AudioRecorder() : buffer(), recording(false), ppDone(false) {
	cout << buffer.getDevice() << endl;
}

AudioRecorder::~AudioRecorder() {
	if (instance == nullptr) delete instance;
}

AudioRecorder* AudioRecorder::getInstance() {
	if (instance == nullptr) instance = new AudioRecorder();
	return instance;
}

const bool AudioRecorder::isRecording() {
	return recording;
}

void AudioRecorder::toogleState() {
	if (recording) {
		buffer.stop();
		recording = false;
	}
	else recording = buffer.start();
}

const sf::SoundBuffer & AudioRecorder::getRawBuffer()
{
	return buffer.getBuffer();
}

const sf::SoundBuffer & AudioRecorder::applyLpf(float lpfCuttof)
{
	//Initialize the PPbuffer
	ppBuffer = sf::SoundBuffer();

	const unsigned int sampleRate = buffer.getBuffer().getSampleRate()*buffer.getChannelCount();
	const float deltaCoef = 1 / sampleRate;
	const sf::Uint64 nbSamples = buffer.getBuffer().getSampleCount();

	//Working sample collection (RBJ using float values)
	vector<float> tmpfArray(buffer.getBuffer().getSamples(), buffer.getBuffer().getSamples() + nbSamples);

	///Apply LFP
	//Channels (at this point, we should work with a mono PCM signal)
	float* audioData[1];
	audioData[0] = tmpfArray.data();

	//RBJ Biquad Low Pass
	Dsp::SmoothedFilterDesign <Dsp::RBJ::Design::LowPass, 1> f(1024);
	Dsp::Params params;
	params[0] = 44100;		// sample rate
	params[1] = lpfCuttof;	// cutoff frequency
	params[2] = 0.7;		// Q
	f.setParams(params);
	f.process(nbSamples, audioData);

	//Recast to Int16
	vector<sf::Int16> ppData;
	ppData.resize(nbSamples);

	for (int i(0); i < nbSamples; i++) {
		ppData[i] = tmpfArray[i];
	}

	//Load in buffer
	ppBuffer.loadFromSamples(ppData.data(), tmpfArray.size(), buffer.getChannelCount(), buffer.getSampleRate());

	return ppBuffer;
}

const sf::SoundBuffer& AudioRecorder::downsample(float newSampleRate) {

	ppBuffer = sf::SoundBuffer(buffer.getBuffer());

	const unsigned int sampleRate = buffer.getBuffer().getSampleRate()*buffer.getChannelCount();

	const unsigned int scaleFactor = sampleRate / newSampleRate;
	
	const sf::Int16* legacySamples = ppBuffer.getSamples();
	const unsigned int samplesCount = ppBuffer.getSampleCount();


	vector<sf::Int16> newSamples = vector<sf::Int16>();

	//Resampling routine
	for (unsigned int i(0); i < samplesCount; i += scaleFactor) {
		sf::Int16 meanSample = 0;

		for (unsigned int j(0); j < scaleFactor; j++) {
			meanSample += legacySamples[scaleFactor*i + j];
		}

		meanSample /= scaleFactor;
		newSamples.emplace_back(meanSample);
	}

	//Remake the buffer
	ppBuffer = sf::SoundBuffer();
	ppBuffer.loadFromSamples(newSamples.data(), newSamples.size(), 1, newSampleRate);

	return ppBuffer;
}

void AudioRecorder::computePostProcessing() {
	if (!ppDone) {
		///Stereo to mono


		///LPF
		applyLpf(5000);

		///Downsampling
		downsample(11025);
	}
	ppDone = true;
}
