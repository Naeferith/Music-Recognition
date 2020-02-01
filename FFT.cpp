#include "FFT.h"

const vector<int> FFT::RANGE = { 10, 20, 40, 80, 160, 511 };
const int FFT::FUZZ_FACTOR = 2;

FFT::FFT(const SoundBuffer& buffer, int const& _bufferSize) : buffer(buffer)
{
	//Set drawing primitives
	VA1.setPrimitiveType(LineStrip);
	VA2.setPrimitiveType(Lines);
	VA3.setPrimitiveType(LineStrip);

	//Member affectation
	sampleRate = buffer.getSampleRate()*buffer.getChannelCount();
	sampleCount = buffer.getSampleCount();
	bufferSize = (_bufferSize < sampleCount) ? _bufferSize : sampleCount;
	mark = 0;

	//Hamming Window
	for(int i(0); i < bufferSize; i++) window.push_back(0.54f-0.46f*(float)cos(2*PI*i/(float)bufferSize));

	//Collections resizing
	sample.resize(bufferSize);
	VA1.resize(bufferSize);

	//Audio loading
	sound.setBuffer(buffer);
	//sound.setLoop(true);
	sound.play();
}

void FFT::hammingWindow()
{
	mark = sound.getPlayingOffset().asSeconds()*sampleRate;
	if(mark + bufferSize < sampleCount)
	{
		for(int i(mark); i < bufferSize +mark; i++)
		{
			sample[i-mark] = Complex(buffer.getSamples()[i]*window[i-mark],0);
			VA1[i-mark] = Vertex(Vector2f(20.f,250.f)+Vector2f((i-mark)/(float)bufferSize *700,sample[i-mark].real()*0.005f),Color::Color(255,0,0,250));
		}
	}
}

void FFT::fft(CArray &x)
{
	const size_t N = x.size();
	if(N <= 1) return;

	CArray even = x[slice(0,N/2,2)];
	CArray  odd = x[slice(1,N/2,2)];

	fft(even);
	fft(odd);

	for(int k = 0; k < N/2; k++)
	{
		Complex t = polar(1.0,-2 * PI * k / N) * odd[k];
		x[k] = even[k] + t;
		x[k+N/2] = even[k] - t;
	}
}

void FFT::update()
{
	hammingWindow();

	VA2.clear();
	VA3.clear();

	bin = CArray(sample.data(),bufferSize);
	fft(bin);

	computeKeyPoints(bin);

	float max = 100000000;
	
	lines(max);
	bars(max);
}

void FFT::bars(float const& max)
{
	Vector2f position(0,800);
	for(float i(3); i < min(bufferSize/2.f,20000.f); i*=1.01f)
	{
		Vector2f samplePosition(log(i)/log(min(bufferSize/2.f,20000.f)),(float)abs(bin[(int)i]));

		//Draw Bars
		VA2.append(Vertex(position+Vector2f(samplePosition.x*800,-samplePosition.y/max*4000),Color::White));
		VA2.append(Vertex(position+Vector2f(samplePosition.x*800,0),Color::White));

		//Draw Bars Reflection
		VA2.append(Vertex(position+Vector2f(samplePosition.x*800,0),Color::Color(255,255,255,100)));
		VA2.append(Vertex(position+Vector2f(samplePosition.x*800,samplePosition.y/max*500/2.f),Color::Color(255,255,255,0)));
	}
}
void FFT::lines(float const& max)
{
	Vector2f position(0,800);
	Vector2f samplePosition;
	float colorDecay = 1;
	double MAXLENGTH = 3e5;

	//Remove oldest elements
	while (cascade.size() > MAXLENGTH) {
		cascade.pop_front();
	}

	//Update existing lines positions
	for(float i(0); i < cascade.size(); i++)
	{
		cascade[i].position -= Vector2f(-0.8f,1.f);
		if(cascade[i].color.a != 0) cascade[i].color = Color(255,255,255,20);
	}

	samplePosition = Vector2f(log(3.f)/log(min(bufferSize/2.f,20000.f)),(float)abs(bin[(int)3]));
	cascade.push_back(Vertex(position+Vector2f(samplePosition.x*800,-samplePosition.y/max*500),Color::Transparent));

	for(float i(3); i < bufferSize/2.f; i*=1.02f)
	{
		samplePosition = Vector2f(log(i)/log(min(bufferSize/2.f,20000.f)),(float)abs(bin[(int)i]));
		cascade.push_back(Vertex(position+Vector2f(samplePosition.x*800,-samplePosition.y/max*500),Color::Color(255,255,255,20)));
	}

	cascade.push_back(Vertex(position+Vector2f(samplePosition.x*800,-samplePosition.y/max*500),Color::Transparent));

	for(int i(std::max((double)0,cascade.size()- MAXLENGTH)); i < cascade.size(); i++) VA3.append(cascade[i]);
}

void FFT::draw(RenderWindow &window)
{
	window.draw(VA1); //Amplitude
	window.draw(VA3); //FTT shadow
	window.draw(VA2); //FTT Graph
}

void FFT::printMagnitudes()
{
	for (int i(0); i < bufferSize; i++) cout << log(abs(bin[i])) << endl;
}

void FFT::computeKeyPoints(CArray& data)
{
	vector<double> highscores = vector<double>(RANGE.size(), 0);
	vector<int> recordPoints = vector<int>(RANGE.size(), 0);

	//foreach bin of frequencies
	for (int freq(0); freq < data.size() / 2; freq++) {
		//get the magnitude of the bin
		double mag = abs(data[freq]);

		//get the band index where this frequency belongs
		int index = getIndex(freq);

		//store the data if it's the current most powerful magnitude
		if (mag > highscores[index]) {
			highscores[index] = mag;
			recordPoints[index] = freq;
		}
	}

	double mean = 0;
	cout << "---------------- DATA at this time ----------- \nPowerful BINS   : ";
	for (int j(0); j < highscores.size(); j++) cout << recordPoints[j] << "\t";
	cout << "\nAmplitude       : ";
	for (int j(0); j < highscores.size(); j++) {
		mean += highscores[j];
		cout << highscores[j] << "\t";
	}

	cout << "\nAverage Ampl.   : ";
	cout << mean/highscores.size() << "\n";

	cout << "\nFILTERED FREQ.  : ";
	vector<int> filter;
	for (int j(0); j < highscores.size(); j++) if (highscores[j] > mean / highscores.size()) filter.emplace_back(recordPoints[j]);
	for (auto &bin : filter)
	{
		cout << bin << "\t";
	}
	cout << endl << "HASH : "<< hashRVR(filter) << endl;



	/*int nbOfIterations = buffer.getSampleCount()/bufferSize;

	for (int i(0); i < nbOfIterations; i++) {

		vector<double> highscores = vector<double>(RANGE.size(), 0);
		vector<int> recordPoints = vector<int>(RANGE.size(), 0);

		for (int j(0); j < bufferSize; j++) sample[j] = Complex(buffer.getSamples()[j+i*bufferSize] * window[j], 0);

		CArray lineOfData = CArray(sample.data(), bufferSize);
		fft(lineOfData);

		//foreach bin of frequencies
		for (int freq(0); freq < lineOfData.size()/2; freq++) {
			//get the magnitude of the bin
			double mag = log(abs(lineOfData[freq]));

			//get the band index where this frequency belongs
			int index = getIndex(freq);

			//store the data if it's the current most powerful magnitude
			if (mag > highscores[index]) {
				highscores[index] = mag;
				recordPoints[index] = freq;
			}
		}

		cout << "Chunk of data No." << i << " : " << endl;
		for (int j(0); j < highscores.size(); j++) cout << recordPoints[j] << "\t";
		cout << "\n";
	}*/

}

int FFT::getIndex(int freq)
{
	if (freq > RANGE.back()) return RANGE.back();
	int i = 0;
	while (RANGE[i] < freq) i++;
	return i;
}

long long FFT::hashRVR(vector<int>& filter)
{
	long long hash = 0;
	for (auto& bin : filter) {
		int index = getIndex(bin);
		hash += bin*(long long)pow(1000, index);
	}
	return hash;
}
