#include "ModelFFT.h"

#include <set>
#include <algorithm>
#include <functional>
#include "Database.h"

const vector<int> ModelFFT::RANGE = { 10, 20, 40, 80, 160, 511 };

ModelFFT::ModelFFT(const SoundBuffer & buffer, int const & _bufferSize) : buffer(buffer)
{
	//Member affectation
	sampleRate = buffer.getSampleRate()*buffer.getChannelCount();
	sampleCount = buffer.getSampleCount();
	bufferSize = (_bufferSize < sampleCount) ? _bufferSize : sampleCount;
	mark = 0;

	//Hamming Window
	for (int i(0); i < bufferSize; i++) window.push_back(0.54f - 0.46f*(float)cos(2 * MPI*i / (float)bufferSize));

	//Collections resizing
	sample.resize(bufferSize);
}

void ModelFFT::hammingWindow(float offset)
{
	mark = offset*sampleRate;
	if (mark + bufferSize < sampleCount)
	{
		for (int i(mark); i < bufferSize + mark; i++) sample[i - mark] = Complex(buffer.getSamples()[i] * window[i - mark], 0);
	}
}

void ModelFFT::fft(CArray & x)
{
	const size_t N = x.size();
	if (N <= 1) return;

	CArray even = x[slice(0, N / 2, 2)];
	CArray  odd = x[slice(1, N / 2, 2)];

	fft(even);
	fft(odd);

	for (int k = 0; k < N / 2; k++)
	{
		Complex t = polar(1.0, -2 * MPI * k / N) * odd[k];
		x[k] = even[k] + t;
		x[k + N / 2] = even[k] - t;
	}
}

void ModelFFT::load(map<long long, vector<DataPoint>>& hashes, int songId)
{
	const int unitOfTime = 100; //100 milis = 0.1sec
	const int duration = buffer.getDuration().asMilliseconds();

	//Calcul des hash sur toute la chanson
	for (float milis(0); milis < duration; milis += unitOfTime) {
		//Recuperation des samples en fonction du temps
		hammingWindow(milis/1000);
		bin = CArray(sample.data(), bufferSize);
		fft(bin);

		//Generation du hash et stockage
		long long hash = computeKeyPoints(bin);
		if (hashes.find(hash) != hashes.end()) hashes[hash].push_back(DataPoint(songId, milis));
		else hashes[hash] = { DataPoint(songId, milis) };
	}
}

int ModelFFT::compare()
{
	typedef std::function<bool(std::pair<int, int>, std::pair<int, int>)> Comparator;

	Comparator comp =
		[](std::pair<int, int> elem1, std::pair<int, int> elem2)
		{
			return elem1.second > elem2.second;
		};

	map<int, int> matches;

	//matching algo--------------------
	auto hashes = Database::getInstance()->getHashes();

	const int unitOfTime = 100; //100 milis = 0.1sec
	const int duration = buffer.getDuration().asMilliseconds();

	//Calcul des hash sur toute la chanson
	for (float milis(0); milis < duration; milis += unitOfTime) {
		//Recuperation des samples en fonction du temps
		hammingWindow(milis / 1000);
		bin = CArray(sample.data(), bufferSize);
		fft(bin);

		//Generation du hash et stockage
		long long hash = computeKeyPoints(bin);
		if (hashes.find(hash) != hashes.end()) {
			for (auto& datapoint : hashes[hash]) {
				if (matches.find(datapoint.getSongId()) != matches.end()) matches[datapoint.getSongId()]++;
				else matches[datapoint.getSongId()] = 1;
			}
		}
	}

	//---------------------------------

	if (matches.size() == 0) return -1;

	set<pair<int, int>, Comparator> sortedSet(matches.begin(), matches.end(), comp);
	return (*(sortedSet.begin())).first;
}

int ModelFFT::stepCompare(const sf::Int16 * samples, std::size_t sampleCount, int const & _bufferSize)
{
	return 0;
}

long long ModelFFT::computeKeyPoints(CArray & data)
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

	//Compute the mean amplitude force
	double mean = 0;
	for (int j(0); j < highscores.size(); j++) mean += highscores[j];
	mean /= highscores.size();

	//Filter only amplitudes above the mean
	vector<int> filter;
	for (int j(0); j < highscores.size(); j++) if (highscores[j] > mean) filter.emplace_back(recordPoints[j]);

	//return the hash of those string frequencies
	return hashRVR(filter);
}

int ModelFFT::getIndex(int freq)
{
	if (freq > RANGE.back()) return RANGE.back();
	int i = 0;
	while (RANGE[i] < freq) i++;
	return i;
}

long long ModelFFT::hashRVR(vector<int>& filter)
{
	long long hash = 0;
	for (auto& bin : filter) {
		int index = getIndex(bin);
		hash += bin * (long long)pow(1000, index);
	}
	return hash;
}