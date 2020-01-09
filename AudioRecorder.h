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

		/* Change l'�tat de l'enregistreur (lecture/ecriture)*/
		void toogleState();

		//Wrapper
		const sf::SoundBuffer& getRawBuffer();

		/* Applique un filtre passe-bas sur le buffer enregistr�*/
		const sf::SoundBuffer& applyLpf(float lpfCuttof);

		/* R�duit la fr�quence d'�chantillonage du buffer enregistr�*/
		const sf::SoundBuffer& downsample(float newSampleRate);

		/* Applique le traitement de conversion d'un son qualit� CD vers un son qualit� T�l�phone. */ //Le r�sultat est enregistr� dans le membre ppBuffer.
		void computePostProcessing();
		
};