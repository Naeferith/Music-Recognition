#include <DirectXMath.h>
#include <SFML/Graphics.hpp>

class Graph : public sf::Drawable
{
public:
	Graph();
	~Graph();


private:
	void computeFFT();
};

Graph::Graph()
{
}

Graph::~Graph()
{
}

void Graph::computeFFT()
{
	
}
