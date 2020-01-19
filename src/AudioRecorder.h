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
		/* Le destructeur.*/
		~AudioRecorder();

		/* Accesseur sur le singleton.*/
		static AudioRecorder* getInstance();

		/* Accesseur sur recording.*/
		const bool isRecording();

		/* Change l'état de l'enregistreur (lecture/ecriture)*/
		void toogleState();

		//Wrapper
		const sf::SoundBuffer& getRawBuffer();

		/* Applique un filtre passe-bas sur le buffer enregistré*/
		const sf::SoundBuffer& applyLpf(float lpfCuttof);

		/* Réduit la fréquence d'échantillonage du buffer enregistré*/
		const sf::SoundBuffer& downsample(float newSampleRate);

		/* Applique le traitement de conversion d'un son qualité CD vers un son qualité Téléphone. */ //Le résultat est enregistré dans le membre ppBuffer.
		void computePostProcessing();
		
};