#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <complex>
#include <valarray>
#include <math.h>
#include <deque>

/// CONSTANTS POWER OF 2
#define POW2_10 1024
#define POW2_11 2048
#define POW2_12 4096
#define POW2_13 8192
#define POW2_14 16384
#define POW2_15 32768

const double PI = 3.141592653589793238460;

using namespace std;
using namespace sf;

typedef complex<double> Complex;
typedef valarray<Complex> CArray;

class FFT
{
public:
	FFT(const SoundBuffer& buffer,int const& _bufferSize);

	void hammingWindow();
	void fft(CArray &x); //Implementation of the radix 2 Cooley–Tukey algorithm
	void update();

	void bars(float const& max);
	void lines(float const& max);

	void draw(RenderWindow &window);

private:
	SoundBuffer buffer;
	Sound sound;

	vector<Complex> sample;
	vector<float> window;
	CArray bin;

	VertexArray VA1;
	VertexArray VA2;
	VertexArray VA3;
	deque<Vertex> cascade;
	
	int sampleRate;
	int sampleCount;
	int bufferSize;
	int mark;
};

