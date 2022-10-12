#pragma once

#include <fstream>

#include <SDL.h>
#include <SDL_TTF.h>

#include <map>

#include "defines.h"

/**
* \class Get SDL_Texture* from string
* \brief Get SDL_Texture* form string by giving color, font size, font and more
* The class must be initialized with the init fnc in the World class.
* Use the getText fnc to get the SDL_Texture* that you need
* TIP: Use only for changing values, for static:
* use a software to create it and load it like a normal Texture
*/
class Writer
{
public:
	Writer();
	~Writer();
	
	static pair<int2, SDL_Texture*> getText(string text, FONT fontName, COLOR colorName, int fontSize);

	void init();

private:
	static map<int, TTF_Font*> m_cinzel;
	static map<COLOR, SDL_Color> m_colors;

	string m_cinzelLoc;
};

pair<int2, SDL_Texture*> getText(string text, FONT fontName, COLOR colorName, int fontSize);