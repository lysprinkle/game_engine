#include "Renderer.h"

void Renderer::Draw_UI() {
	std::stable_sort(ui_to_be_render.begin(), ui_to_be_render.end(), [](const UIDB::UIDrawRequest& a, const UIDB::UIDrawRequest& b) {
		if (a.sorting_order != b.sorting_order) {
			return a.sorting_order < b.sorting_order;;
		}
		return a.image_order < b.image_order;
		});

	for (auto& ui : ui_to_be_render) {

		std::string cur_image = "resources/images/" + ui.ui_name + ".png";

		SDL_Texture* texture;

		auto it = UIDB::UIs.find(ui.ui_name);
		if (it != UIDB::UIs.end()) {
			texture = it->second;
		}
		else {
			texture = IMG_LoadTexture(renderer, cur_image.c_str());
			UIDB::UIs.emplace(ui.ui_name, texture);
		}

		int intX = static_cast<int>(ui.x);
		int intY = static_cast<int>(ui.y);

		SDL_Rect dstRect;

		dstRect.x = intX;
		dstRect.y = intY;

		int w = 0, h = 0;
		SDL_QueryTexture(texture, NULL, NULL, &w, &h);

		dstRect.w = w;
		dstRect.h = h;

		if (ui.ex) {
			SDL_SetTextureColorMod(texture, static_cast<Uint8>(ui.r), static_cast<Uint8>(ui.g), static_cast<Uint8>(ui.b));
			SDL_SetTextureAlphaMod(texture, static_cast<Uint8>(ui.a));

			Helper::SDL_RenderCopyEx498(0, "0", renderer, texture, NULL, &dstRect, 0, NULL, SDL_FLIP_NONE);

			SDL_SetTextureColorMod(texture, 255, 255, 255);
			SDL_SetTextureAlphaMod(texture, 255);
		}
		else {
			Helper::SDL_RenderCopyEx498(0, "0", renderer, texture, NULL, &dstRect, 0, NULL, SDL_FLIP_NONE);
		}
	}

	ui_order = 0;
	ui_to_be_render.clear();
}

void Renderer::Draw_Text() {
	for (auto& text : text_to_be_render) {
		SDL_Color color = { static_cast<Uint8>(text.r), static_cast<Uint8>(text.g), static_cast<Uint8>(text.b), static_cast<Uint8>(text.a) };
		SDL_Surface* surface = TTF_RenderText_Solid(text.font, text.str_content.c_str(), color);
		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_Rect textRect = { text.x, text.y, surface->w, surface->h };
		SDL_RenderCopy(renderer, texture, nullptr, &textRect);
	}
	text_to_be_render.clear();
}

void Renderer::Draw_Image(float camera_offset_x, float camera_offset_y, int x_resolution_screen, int y_resolution_screen, float zoom_factor) {
	std::stable_sort(image_to_be_render.begin(), image_to_be_render.end(), [](const ImageDB::ImageDrawRequest& a, const ImageDB::ImageDrawRequest& b) {
		if (a.sorting_order != b.sorting_order) {
			return a.sorting_order < b.sorting_order;;
		}
		return a.image_order < b.image_order;
		});

	for (auto& image : image_to_be_render) {
		std::string cur_image = "resources/images/" + image.image_name + ".png";

		SDL_Texture* texture;

		auto it = ImageDB::images.find(image.image_name);
		if (it != ImageDB::images.end()) {
			texture = it->second;
		}
		else {
			texture = IMG_LoadTexture(renderer, cur_image.c_str());
			ImageDB::images.emplace(image.image_name, texture);
		}

		int w, h;

		SDL_QueryTexture(texture, NULL, NULL, &w, &h);

		int pivot_x = static_cast<int>(image.pivot_x * (w * abs(image.scale_x)));
		int pivot_y = static_cast<int>(image.pivot_y * (h * abs(image.scale_y)));

		int x = static_cast<int>((image.x - camera_offset_x) * 100 - pivot_x + x_resolution_screen * 0.5f * (1.0f / zoom_factor));
		int y = static_cast<int>((image.y -camera_offset_y) * 100 - pivot_y + y_resolution_screen * 0.5f * (1.0f / zoom_factor));

		SDL_Point pivot = { static_cast<int>(pivot_x), static_cast<int>(pivot_y) };
		SDL_Rect dstRect;

		dstRect.x = static_cast<int>(x);
		dstRect.y = static_cast<int>(y);

		dstRect.w = w * abs(image.scale_x);
		dstRect.h = h * abs(image.scale_y);


		SDL_RendererFlip flip = SDL_FLIP_NONE;
		if (image.scale_x < 0) flip = SDL_RendererFlip(flip | SDL_FLIP_HORIZONTAL);
		if (image.scale_y < 0) flip = SDL_RendererFlip(flip | SDL_FLIP_VERTICAL);

		SDL_RenderSetScale(renderer, zoom_factor, zoom_factor);

		if (image.ex) {
			SDL_SetTextureColorMod(texture, static_cast<Uint8>(image.r), static_cast<Uint8>(image.g), static_cast<Uint8>(image.b));
			SDL_SetTextureAlphaMod(texture, static_cast<Uint8>(image.a));

			Helper::SDL_RenderCopyEx498(0, "0", renderer, texture, NULL, &dstRect, image.rotation_degrees, &pivot, flip);

			SDL_SetTextureColorMod(texture, 255, 255, 255);
			SDL_SetTextureAlphaMod(texture, 255);
		}
		else {

			Helper::SDL_RenderCopyEx498(0, "0", renderer, texture, NULL, &dstRect, 0, NULL, flip);
		}

		SDL_RenderSetScale(renderer, 1.0f, 1.0f);
	}

	image_order = 0;
	image_to_be_render.clear();
}

void Renderer::Draw_Pixel() {
	for (auto& image : pixel_to_be_render) {
		SDL_BlendMode prevBlendMode;
		Uint8 prevR, prevG, prevB, prevA;
		SDL_GetRenderDrawBlendMode(renderer, &prevBlendMode);
		SDL_GetRenderDrawColor(renderer, &prevR, &prevG, &prevB, &prevA);
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(renderer, image.r, image.g, image.b, image.a);
		SDL_RenderDrawPoint(renderer, image.x, image.y);
		SDL_SetRenderDrawBlendMode(renderer, prevBlendMode);
		SDL_SetRenderDrawColor(renderer, prevR, prevG, prevB, prevA);
	}

	pixel_to_be_render.clear();
}

void Renderer::Draw_All(float camera_offset_x, float camera_offset_y, int x_resolution_screen, int y_resolution_screen, float zoom_factor) {
	Draw_Image(camera_offset_x, camera_offset_y, x_resolution_screen, y_resolution_screen, zoom_factor);
	Draw_UI();
	Draw_Text();
	Draw_Pixel();
}

namespace renderer {
	void DrawUI(std::string image_name, float x, float y) {
		UIDB::UIDrawRequest cur_ui;
		cur_ui.ui_name = image_name;
		cur_ui.x = x;
		cur_ui.y = y;
		cur_ui.sorting_order = 0;
		cur_ui.image_order = Renderer::ui_order++;
		cur_ui.ex = false;

		Renderer::ui_to_be_render.emplace_back(cur_ui);
	}

	void DrawUIEx(std::string image_name, float x, float y, float r, float g, float b, float a, int sorting_order) {
		UIDB::UIDrawRequest cur_ui;
		cur_ui.ui_name = image_name;
		cur_ui.x = x;
		cur_ui.y = y;
		cur_ui.r = r;
		cur_ui.g = g;
		cur_ui.b = b;
		cur_ui.a = a;
		cur_ui.sorting_order = sorting_order;
		cur_ui.image_order = Renderer::ui_order++;
		cur_ui.ex = true;

		Renderer::ui_to_be_render.emplace_back(cur_ui);
	}

	void DrawText(std::string str_content, float x, float y, std::string font_name, float font_size, float r, float g, float b, float a) {
		std::string font_path = "resources/fonts/" + font_name + ".ttf";

		TTF_Font* font;
		auto it = TextDB::texts.find(font_name);
		if (it != TextDB::texts.end()) {
			font = it->second;
		}
		else {
			font = TTF_OpenFont(font_path.c_str(), font_size);
			TextDB::texts.emplace(font_name, font);
		}

		TextDB::TextDrawRequest cur_text;
		cur_text.x = x;
		cur_text.y = y;
		cur_text.font = font;
		cur_text.font = font;
		cur_text.str_content = str_content;
		cur_text.r = r;
		cur_text.g = g;
		cur_text.b = b;
		cur_text.a = a;

		Renderer::text_to_be_render.emplace_back(cur_text);
	}

	void DrawImage(std::string image_name, float x, float y) {
		ImageDB::ImageDrawRequest cur;
		cur.image_name = image_name;
		cur.x = x;
		cur.y = y;
		cur.sorting_order = 0;
		cur.image_order = Renderer::image_order++;
		cur.ex = false;

		Renderer::image_to_be_render.emplace_back(cur);
	}

	void DrawEx(std::string image_name, float x, float y, float rotation_degrees, float scale_x, float scale_y, float pivot_x, float pivot_y, float r, float g, float b, float a, int sorting_order) {
		ImageDB::ImageDrawRequest cur;
		cur.image_name = image_name;
		cur.x = x;
		cur.y = y;
		cur.rotation_degrees = static_cast<int>(rotation_degrees);
		cur.scale_x = scale_x;
		cur.scale_y = scale_y;
		cur.pivot_x = pivot_x;
		cur.pivot_y = pivot_y;
		cur.r = static_cast<int>(r);
		cur.g = static_cast<int>(g);
		cur.b = static_cast<int>(b);
		cur.a = static_cast<int>(a);
		cur.sorting_order = static_cast<int>(sorting_order);
		cur.image_order = Renderer::image_order++;
		cur.ex = true;

		Renderer::image_to_be_render.emplace_back(cur);
	}

	void DrawPixel(float x, float y, float r, float g, float b, float a) {

		int intX = static_cast<int>(x);
		int intY = static_cast<int>(y);
		int intR = static_cast<int>(r);
		int intG = static_cast<int>(g);
		int intB = static_cast<int>(b);
		int intA = static_cast<int>(a);

		PixelDB::PixelDrawRequest cur_pixel;
		cur_pixel.x = intX;
		cur_pixel.y = intY;
		cur_pixel.r = intR;
		cur_pixel.g = intG;
		cur_pixel.b = intB;
		cur_pixel.a = intA;

		Renderer::pixel_to_be_render.push_back(cur_pixel);
	}
}