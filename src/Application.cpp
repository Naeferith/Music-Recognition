#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "AudioRecorder.h"
#include "FFT.h"
#include "Database.h"
#include "ModelFFT.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#define CLOCK_DEBUG

int main()
{
	sf::RenderWindow window(sf::VideoMode(900, 900), "Spectrogram");

	sf::Color discordColor(54,57,63);

	//The application rely on real-time audio capture. If this feature is not avaliable on this system
	//we cant use this application
	if (sf::SoundBufferRecorder::isAvailable()) {
		
		AudioRecorder* audioRecorder = AudioRecorder::getInstance();

		sf::Sound soundPlayer = sf::Sound();

		///LOAD DATABASE // CAN BE LONG
		Database* BDD = Database::getInstance();

		//FFT* fft = nullptr;

		int id = -1;
		ModelFFT* mfft = nullptr;

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
					case sf::Keyboard::Enter: {
						if (mfft != nullptr) delete mfft;
						mfft = new ModelFFT(audioRecorder->getRawBuffer(), POW2_10); // <-- Raw audio

#ifdef CLOCK_DEBUG
#include<chrono>
						auto start = chrono::high_resolution_clock::now();

						id = mfft->compare();

						auto stop = chrono::high_resolution_clock::now();
						auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);

						cout << "COMPARE : " << duration.count() << " miliseconds" << endl;
#else
						id = mfft->compare();
#endif // CLOCK_DEBUG

						cout << endl << ((id == -1) ? "Nothing found." : "Best Match : " + BDD->getSongName(id)) << endl;
						break;
						/*case sf::Keyboard::Numpad0:
							if (fft != nullptr) delete fft;
							fft = new FFT(audioRecorder->getRawBuffer(), POW2_10); // <-- Raw audio
							break;
						case sf::Keyboard::Numpad1:
							if (fft != nullptr) delete fft;
							fft = new FFT(audioRecorder->applyLpf(5000), POW2_10); // <-- Post Processed Audio
							break;*/
						/*case sf::Keyboard::Numpad2:
							if (fft != nullptr) delete fft;
							fft = new FFT(fileBuffer, POW2_12); // <-- Raw audio
							//fft->computeKeyPoints();
							break;*/
						/*case sf::Keyboard::Numpad3:
							if (fft != nullptr) delete fft;
							audioRecorder->loadBufferFromFile("Database/Songs/ass.wav");
							audioRecorder->computePostProcessing();
							fft = new FFT(audioRecorder->getPpBuffer(), POW2_10); // <-- Raw audio
							//fft->computeKeyPoints();
							break;*/
					}
					default:
						break;
					}
				}
			}

			//if (fft != nullptr && fft->sound.getStatus() == Sound::Playing) fft->update();

			window.clear();
			//if (fft != nullptr) fft->draw(window);
			window.display();
		}
	}

	
	//_CrtDumpMemoryLeaks();
	return 0;
}