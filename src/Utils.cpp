#include <iostream>
#include <sstream>
#include <string>
#include <chrono>
#include <thread>
#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"
#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"
#include "Utils.h"
#include "Helper.h"

void ReadJsonFile(const std::string& path, rapidjson::Document& out_document)
{
	FILE* file_pointer = nullptr;
#ifdef _WIN32
	fopen_s(&file_pointer, path.c_str(), "rb");
#else
	file_pointer = fopen(path.c_str(), "rb");
#endif
	char buffer[65536];
	rapidjson::FileReadStream stream(file_pointer, buffer, sizeof(buffer));
	out_document.ParseStream(stream);
	std::fclose(file_pointer);

	if (out_document.HasParseError()) {
		rapidjson::ParseErrorCode errorCode = out_document.GetParseError();
		std::cout << "error parsing json at [" << path << "]" << std::endl;
		exit(0);
	}
}

uint64_t get_packed_value(int x, int y) {
	uint64_t key = (static_cast<uint64_t>(x) << 32) + y;
	return key;
}

void establishInheritance(luabridge::LuaRef& instance_table, luabridge::LuaRef& parent_table, lua_State* lua_state)
{
	/* We must create a metatable to establish inheritance in Lua. */
	luabridge::LuaRef new_metatable = luabridge::newTable(lua_state);
	new_metatable["__index"] = parent_table;

	/* We must use the raw lua C-API (Lua stack) to perform a "setmetatable" operation. */
	instance_table.push(lua_state);
	new_metatable.push(lua_state);
	lua_setmetatable(lua_state, -2);
	lua_pop(lua_state, 1);
}

std::string get_lua_file_location(std::string lua_file) {
	std::string lua_location = "resources/component_types/" + lua_file + ".lua";
	return lua_location;
}

void Quit() {
	exit(0);
}

int GetFrame() {
	return Helper::GetFrameNumber();
}

void Sleep(int dur_ms) {
	std::this_thread::sleep_for(std::chrono::milliseconds(dur_ms));
}

void OpenURL(std::string url) {
	std::string command = "start " + url;
	std::system(command.c_str());
}

