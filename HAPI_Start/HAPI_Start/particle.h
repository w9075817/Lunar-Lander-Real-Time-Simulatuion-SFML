#pragma once
#include <vector>
#include "HAPI_lib.h"
using namespace HAPISPACE;
class particle
{
public:
	particle::particle();

	std::vector<int> m_particleVectors;
	void draw_particle(int posX, int posy, int screenWidth, BYTE* screen);
};

