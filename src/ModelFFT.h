#pragma once

#include <SFML/Audio.hpp>
#include <iostream>
#include <complex>
#include <valarray>
#include <math.h>
#include <map>

#include "DataPoint.h"

/// CONSTANTS POWER OF 2
#define POW2_10 1024
#define POW2_11 2048
#define POW2_12 4096
#define POW2_13 8192
#define POW2_14 16384
#define POW2_15 32768

const double MPI = 3.141592653589793238460;

using namespace std;
using namespace sf;

typedef complex<double> Complex;
typedef valarray<Complex> CArray;


class ModelFFT {
private:
	SoundBuffer buffer;

	vector<Complex> sample;
	vector<float> window;
	CArray bin;

	int sampleRate;
	int sampleCount;
	int bufferSize;
	int mark;

	static const vector<int> RANGE;
	static int getIndex(int freq);
	static long long hashRVR(vector<int>& filter);

	long long computeKeyPoints(CArray& data);

	/* Offset en secondes*/
	void hammingWindow(float offset);
	void fft(CArray &x); //Implementation of the radix 2 Cooley–Tukey algorithm
public:
	ModelFFT(const SoundBuffer& buffer, int const& _bufferSize);
	
	void load(map<long long, vector<DataPoint>>& hashes, int songId);

	int compare();
	
	int stepCompare();
	void epurate(map<int, vector<int>>& start, int offset, long long nextHash);
};