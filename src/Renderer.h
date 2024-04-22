#pragma once
#include<string>
#include<algorithm>

#include "SDL2/SDL.h"
#include "SDL2_image/SDL_image.h"
#include "ImageDB.h"
#include "UIDB.h"
#include "TextDB.h"
#include "PixelDB.h"
#include "Helper.h"

class Renderer
{
public:
	static inline SDL_Renderer* renderer;
	static inline int* camera_offset_x;
	static inline int* camera_offset_y;
	static inline int* x_resolution_screen;
	static inline int* y_resolution_screen;
	static inline float* zoom_factor;

	static inline int image_order = 0;
	static inline std::vector<ImageDB::ImageDrawRequest> image_to_be_render;

	static inline int ui_order = 0;
	static inline std::vector<UIDB::UIDrawRequest> ui_to_be_render;

	static inline int text_order = 0;
	static inline std::vector<TextDB::TextDrawRequest> text_to_be_render;

	static inline int pixel_order = 0;
	static inline std::vector<PixelDB::PixelDrawRequest> pixel_to_be_render;
	
	static void Draw_Image(float, float, int, int, float);

	static void Draw_UI();

	static void Draw_Text();

	static void Draw_Pixel();

	static void Draw_All(float, float, int, int, float);
};

namespace renderer {
	void DrawUI(std::string, float, float);

	void DrawUIEx(std::string, float, float, float, float, float, float, int);

	void DrawText(std::string, float, float, std::string, float, float, float, float, float);

	void DrawImage(std::string image_name, float x, float y);

	void DrawEx(std::string image_name, float x, float y, float rotation_degrees, float scale_x, float scale_y, float pivot_x, float pivot_y, float r, float g, float b, float a, int sorting_order);

	void DrawPixel(float, float, float, float, float, float);
}

