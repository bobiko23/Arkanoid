#pragma once

#include <fstream>
#include <stdlib.h>
#include <time.h>

#include "Engine.h"
#include "defines.h"

const int m_ROWS = 4;
const int m_COLS = 5;

class Brick
{
public:
	Brick();
	~Brick();

	void init();
	void update(SDL_Rect ball);
	void draw();
	
	BrickData m_allBricks [m_ROWS][m_COLS];
};