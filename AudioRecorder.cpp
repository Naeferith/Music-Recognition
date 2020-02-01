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
	else recording = buffer.start(11025);
}

const sf::SoundBuffer & AudioRecorder::getRawBuffer() const
{
	return buffer.getBuffer();
}

const sf::SoundBuffer & AudioRecorder::getPpBuffer() const
{
	return ppBuffer;
}

const sf::SoundBuffer & AudioRecorder::applyLpf(float lpfCuttof)
{
	//Initialize the PPbuffer
	//ppBuffer = sf::SoundBuffer();

	const unsigned int sampleRate = ppBuffer.getSampleRate()*ppBuffer.getChannelCount();
	const float deltaCoef = 1 / sampleRate;
	const sf::Uint64 nbSamples = ppBuffer.getSampleCount();

	//Working sample collection (RBJ using float values)
	vector<float> tmpfArray(ppBuffer.getSamples(), ppBuffer.getSamples() + nbSamples);

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
	ppBuffer.loadFromSamples(ppData.data(), tmpfArray.size(), ppBuffer.getChannelCount(), ppBuffer.getSampleRate());

	return ppBuffer;
}

const sf::SoundBuffer& AudioRecorder::downsample(float newSampleRate) {

	//ppBuffer = sf::SoundBuffer(fileBuffer);

	const unsigned int sampleRate = ppBuffer.getSampleRate()*ppBuffer.getChannelCount();

	const unsigned int scaleFactor = sampleRate / newSampleRate;
	
	const sf::Int16* legacySamples = ppBuffer.getSamples();
	const unsigned int samplesCount = ppBuffer.getSampleCount();


	vector<sf::Int16> newSamples = vector<sf::Int16>();

	//Resampling routine
	for (unsigned int i(0); i < samplesCount-scaleFactor; i += scaleFactor) {
		sf::Int16 meanSample = 0;

		for (unsigned int j(0); j < scaleFactor; j++) {
			meanSample += legacySamples[i + j];
		}

		meanSample /= scaleFactor;
		newSamples.emplace_back(meanSample);
	}

	//Remake the buffer
	ppBuffer = sf::SoundBuffer();
	ppBuffer.loadFromSamples(newSamples.data(), newSamples.size(), 1, newSampleRate);

	return ppBuffer;
}

const sf::SoundBuffer & AudioRecorder::toMono()
{
	const unsigned int chanels = fileBuffer.getChannelCount();
	if (chanels == 2) {
		const sf::Int16* samples = fileBuffer.getSamples();
		const unsigned int samplesCount = fileBuffer.getSampleCount();

		vector<sf::Int16> newSamples;
		newSamples.resize(samplesCount);

		for (unsigned i(0); i < samplesCount; i += 4) {
			short left = *(short*)&samples[i];
			short right = *(short*)&samples[i + 2];
			newSamples[i] = (int(left) + right) / 2;
		}

		//Remake the buffer
		ppBuffer = sf::SoundBuffer();
		ppBuffer.loadFromSamples(newSamples.data(), newSamples.size(), 1, 2*fileBuffer.getSampleRate());
	}
	return ppBuffer;
}

void AudioRecorder::computePostProcessing() {
	//Le fichier doit respecter certaines contraintes
	//Taux d'echantillonage qualité CD (44.1 kHz)
	//Peut etre indifferament Mono ou Stereo

	cout << "Channels    : " << fileBuffer.getChannelCount() << endl;
	cout << "Sample rate : " << fileBuffer.getSampleRate() << endl;

	///(1) Conversion en Mono si le fichier et stereo
	toMono();

	///(2) On applique un filtre passe-bas pour préparer le buffer au downsampling
	applyLpf(5000);

	///(3) Downsample qualité téléphone (11.025 kHz)
	downsample(11025);
}

bool AudioRecorder::loadBufferFromFile(const string & path)
{
	return fileBuffer.loadFromFile(path);
}
