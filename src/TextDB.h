#pragma once

#include<unordered_map>
#include<string>
#include "SDL2_ttf/SDL_ttf.h"

class TextDB
{
public:
	struct TextDrawRequest {
		float x;
		float y;
		float r;
		float g;
		float b;
		float a;
		TTF_Font* font;
		std::string str_content;
	};

	static inline std::unordered_map<std::string, TTF_Font*> texts;
};

