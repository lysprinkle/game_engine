#pragma once
#include <string>
#include <unordered_map>
#include "SDL2_image/SDL_image.h"

class UIDB
{
public:
    struct UIDrawRequest
    {
        std::string ui_name;
        float x;
        float y;
        int rotation_degrees = 0;
        float scale_x = 1.0;
        float scale_y = 1.0;
        float pivot_x = 0.5;
        float pivot_y = 0.5;
        int r;
        int g;
        int b;
        int a;
        int sorting_order;
        int image_order;
        bool ex;
    };

    static inline std::unordered_map<std::string, SDL_Texture*> UIs;
};

