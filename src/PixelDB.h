#ifndef PIXELDB_H
#define PIXELDB_H

#include <unordered_map>
#include <string>

class PixelDB
{
public:
	struct PixelDrawRequest {
		int x;
		int y;
		int r;
		int g;
		int b;
		int a;
	};
};

#endif

