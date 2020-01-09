#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "AudioRecorder.h"
#include "FFT.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

int main()
{
	sf::RenderWindow window(sf::VideoMode(900, 900), "Spectrogram");

	sf::Color discordColor(54,57,63);

	//The application rely on real-time audio capture. If this feature is not avaliable on this system
	//we cant use this application
	if (sf::SoundBufferRecorder::isAvailable()) {
		
		AudioRecorder* audioRecorder = AudioRecorder::getInstance();
		sf::Sound soundPlayer = sf::Sound();

		FFT* fft = nullptr;

		//Window Life Loop
		while (window.isOpen())
		{
			sf::Event event;
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
					window.close();

				if (event.type == sf::Event::KeyPressed) {
					switch (event.key.code)
					{
					case sf::Keyboard::Space:
						audioRecorder->toogleState();
						break;
					case sf::Keyboard::Enter:
						soundPlayer.setBuffer(audioRecorder->getRawBuffer());
						soundPlayer.play();
						break;
					case sf::Keyboard::Numpad0:
						if (fft != nullptr) delete fft;
						fft = new FFT(audioRecorder->getRawBuffer(), POW2_14);
						break;
					case sf::Keyboard::Numpad1:
						if (fft != nullptr) delete fft;
						fft = new FFT(audioRecorder->applyLpf(5000), POW2_14);
						break;
					default:
						break;
					}
				}
			}

			if (fft != nullptr) fft->update();

			window.clear();
			if (fft != nullptr) fft->draw(window);
			window.display();
		}
	}

	
	//_CrtDumpMemoryLeaks();
	return 0;
}