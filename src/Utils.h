#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <sstream>
#include <string>
#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"

void ReadJsonFile(const std::string&, rapidjson::Document&);

uint64_t get_packed_value(int, int);

void establishInheritance(luabridge::LuaRef&, luabridge::LuaRef&, lua_State*);

std::string get_lua_file_location(std::string);

void Quit();

int GetFrame();

void Sleep(int dur_ms);

void OpenURL(std::string url);

#endif