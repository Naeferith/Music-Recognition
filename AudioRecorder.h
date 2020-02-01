#pragma once

#include <SFML/Audio.hpp>

#include "DebugAudioRecorder.h"

using namespace std;

class AudioRecorder
{
	private:
		static AudioRecorder* instance;
		DebugAudioRecorder buffer;

		sf::SoundBuffer fileBuffer, ppBuffer;
		bool ppDone;

		bool recording;

		AudioRecorder();

		


	public:
		/* Le destructeur.*/
		~AudioRecorder();

		/* Accesseur sur le singleton.*/
		static AudioRecorder* getInstance();

		/* Accesseur sur recording.*/
		const bool isRecording();

		/* Change l'état de l'enregistreur (lecture/ecriture)*/
		void toogleState();

		//Wrapper
		const sf::SoundBuffer& getRawBuffer() const;

		//wrapper
		const sf::SoundBuffer& getPpBuffer() const;

		/* Applique un filtre passe-bas sur le buffer enregistré*/
		const sf::SoundBuffer& applyLpf(float lpfCuttof);

		/* Réduit la fréquence d'échantillonage du buffer enregistré*/
		const sf::SoundBuffer& downsample(float newSampleRate);

		/* Transforme le signal en signal mono*/
		const sf::SoundBuffer& toMono();

		/* Applique le traitement de conversion d'un son qualité CD vers un son qualité Téléphone. */ //Le résultat est enregistré dans le membre ppBuffer.
		void computePostProcessing();
		
		bool loadBufferFromFile(const string& path);
};