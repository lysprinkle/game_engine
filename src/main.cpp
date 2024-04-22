#include <iostream>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <filesystem>
#include <string>
#include <set>
#include <algorithm>
#include <chrono>
#include "GameStart.h"
#include "TemplateDB.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"
#include "Utils.h"
#include "Helper.h"
#include "AudioHelper.h"


#include "SDL2/SDL.h"
#include "SDL2_image/SDL_image.h"

#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"

#include "GameStart.h"

void game_start() {

	GameStart* game = new GameStart();
	game->game_initialization();
	game->game_running();
}

int main(int argc, char* argv[]) {


	lua_State* lua_state = luaL_newstate();
	luaL_openlibs(lua_state);

	game_start();

	lua_close(lua_state);
	return 0;
}