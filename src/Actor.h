#pragma once

#include <string>
#include <optional>
#include <unordered_set>
#include <map>
#include <filesystem>
#include "glm/glm.hpp"
#include "SDL2/SDL.h"
#include "SDL2_image/SDL_image.h"
#include "SDL2_ttf/SDL_ttf.h"
#include "SDL2_mixer/SDL_mixer.h"
#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"
#include "Utils.h"

class Actor
{
public:
	static inline int add_time = 0;
	std::string actor_name = "";

	int id;

	bool loaded = false;
	bool live = true;

	std::unordered_set<Actor*> colliding_actors_this_frame;
	std::unordered_set<Actor*> triggering_actors_this_frame;

	std::vector<std::string> template_instances1;
	std::vector<luabridge::LuaRef> template_instances2;
	std::map<std::string, luabridge::LuaRef> componenet_instances;
	std::map<std::string, std::string> componenet_types;
	std::map<std::string, luabridge::LuaRef> components_to_be_added;





	Actor(std::string actor_name, char view, glm::ivec2 position, glm::ivec2 initial_velocity,
		bool blocking, std::string nearby_dialogue, std::string contact_dialogue, int id)
		: actor_name(actor_name), id(id) {}

	Actor() {}

	void InjectConvenienceReferences(std::shared_ptr<luabridge::LuaRef> component_ref)
	{
		(*component_ref)["actor"] = this;
	}

	std::string GetName() {
		return this->actor_name;
	}

	int GetID() {
		return this->id;
	}

	luabridge::LuaRef GetComponentByKey(const std::string& key, lua_State* lua_state) {
		auto it = componenet_instances.find(key);
		if (it != componenet_instances.end() && it->second["enabled"]) {
			return it->second;
		}
		else {
			return luabridge::LuaRef(lua_state);
		}
	}

	luabridge::LuaRef GetComponent(const std::string& type_name, lua_State* lua_state) {
		for (const auto& componenet_type : componenet_types) {
			if (componenet_type.second == type_name) {
				auto it = componenet_instances.find(componenet_type.first);
				if (it != componenet_instances.end() && it->second["enabled"]) {
					return it->second;
				}
			}
		}
		return luabridge::LuaRef(lua_state);
	}


	luabridge::LuaRef GetComponents(const std::string& type_name, lua_State* lua_state) {
		luabridge::LuaRef table = luabridge::newTable(lua_state);
		int index = 1;
		for (const auto& componenet_type : componenet_types) {
			if (componenet_type.second == type_name) {
				auto it = componenet_instances.find(componenet_type.first);
				if (it != componenet_instances.end() && it->second["enabled"]) {
					table[index++] = it->second;
				}
				
			}
		}
		return table;
	}

	luabridge::LuaRef AddComponent(const std::string& type_name, lua_State* lua_state) {
		if (!std::filesystem::exists(get_lua_file_location(type_name))) {
			std::cout << "error: failed to locate component " << type_name;
			exit(0);
		}

		if (luaL_dofile(lua_state, get_lua_file_location(type_name).c_str()) != 0) {
			std::cout << "problem with lua file " << type_name;
			exit(0);
		}

		std::string key = "r" + std::to_string(add_time);
		add_time++;

		luabridge::LuaRef componentInstance = luabridge::newTable(lua_state);
		luabridge::LuaRef componentDef = luabridge::getGlobal(lua_state, type_name.c_str());

		establishInheritance(componentInstance, componentDef, lua_state);

		componentInstance["key"] = key;
		componentInstance["enabled"] = true;

		componenet_types.emplace(key, type_name);

		components_to_be_added.emplace(key, componentInstance);

		return componentInstance;
	}

	void RemoveComponent(luabridge::LuaRef component_remove) {
		component_remove["enabled"] = false;
	}
};
